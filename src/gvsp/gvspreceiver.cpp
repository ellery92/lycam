#include "gvspreceiver.h"
#include "gvspreceiver_p.h"
#include "gvsppacket.h"
#include "gvspmemoryallocator.h"

#include <gvcp/bootstrapregisters.h>

#include <assert.h>
#include <vector>

#define GVSP_BUFFER_SIZE 10000
#define IP_HEADER_SIZE 20
#define UDP_HEADER_SIZE 8
#define PTP_DEVICE "/dev/ptp0"

using namespace Jgv::Gvsp;

namespace {

enum class PacketFormat:uint8_t {
    DATA_LEADER = 1,
    DATA_TRAILER = 2,
    DATA_PAYLOAD = 3,
    ALL_IN = 4
};

uint16_t nextBlockId(uint16_t blockID)
{
    return (blockID == UINT16_MAX) ? 1 : ++blockID;
}

uint16_t previousBlockId(uint16_t blockID)
{
    return (blockID == 1) ? UINT16_MAX : --blockID;
}

uint64_t timestamp(uint64_t high, uint64_t low)
{
    return low | (high << 32);
}

//inline std::size_t dataSizeFromGeometry(const Geometry &geometry) noexcept
//{
//    return (geometry.width * geometry.height * GVSP_PIX_PIXEL_SIZE(geometry.pixelFormat)) / 8;
//}

void setFilter(int sd, uint32_t srcIP, uint32_t srcPort, uint32_t dstIP, uint32_t dstPort)
{
    // tcpdump udp and src host <srcIP> and src port <srcPort> dst host <dstIP> and dst port <dstPort> -dd
    sock_filter bpf_filter[18] = {
        { 0x28, 0, 0, 0x0000000c },
        { 0x15, 15, 0, 0x000086dd },
        { 0x15, 0, 14, 0x00000800 },
        { 0x30, 0, 0, 0x00000017 },
        { 0x15, 0, 12, 0x00000011 },
        { 0x20, 0, 0, 0x0000001a },
        { 0x15, 0, 10, srcIP },
        { 0x28, 0, 0, 0x00000014 },
        { 0x45, 8, 0, 0x00001fff },
        { 0xb1, 0, 0, 0x0000000e },
        { 0x48, 0, 0, 0x0000000e },
        { 0x15, 0, 5, srcPort },
        { 0x20, 0, 0, 0x0000001e },
        { 0x15, 0, 3, dstIP },
        { 0x48, 0, 0, 0x00000010 },
        { 0x15, 0, 1, dstPort },
        { 0x6, 0, 0, 0x00040000 },
        { 0x6, 0, 0, 0x00000000 }
    };

    const sock_fprog bpf_prog = {sizeof(bpf_filter) / sizeof(struct sock_filter), bpf_filter};
    if (setsockopt(sd, SOL_SOCKET, SO_ATTACH_FILTER, &bpf_prog, sizeof(bpf_prog)) != 0) {
        std::perror("GvspSocket failed to set filter");
    }
}

void setRealtime()
{
    if (capng_have_capability(CAPNG_EFFECTIVE, CAP_SYS_NICE)) {
        sched_param param;
        param.sched_priority = 20;
        if (sched_setscheduler(0, SCHED_FIFO, &param) < 0) {
            std::perror("GvspSocket failed to set socket thread realtime");
        }
    }
}

unsigned nicIndexFromAddress(uint32_t ip)
{
    unsigned index = 0;

    ifaddrs *ifaddr = nullptr;

    // obtient la liste des interfaces
    if (getifaddrs(&ifaddr) == -1) {
        return index;
    }

    // on parcourt la liste
    for (ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) {
            continue;
        }

        // IPv4
        if (AF_INET != ifa->ifa_addr->sa_family) {
            continue;
        }

        sockaddr_in *sa = reinterpret_cast<sockaddr_in *>(ifa->ifa_addr);
        // contient bien notre IP
        if (ip != ntohl( sa->sin_addr.s_addr) ) {
            continue;
        }

        // l'index de l'interface
        index = if_nametoindex(ifa->ifa_name);
    }

    freeifaddrs(ifaddr);

    return index;
}

//void threadToLastCore() {
//    const unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
//    if (concurentThreadsSupported == 0) {
//        std::cerr << "GvspSocket failed to get hardware concurrency" << std::endl;
//        return;
//    }


//    cpu_set_t cpuset;
//    CPU_ZERO(&cpuset);
//    CPU_SET(concurentThreadsSupported - 1, &cpuset);

//    pthread_t current_thread = pthread_self();
//    int s = pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
//    if (s != 0) {
//        std::perror("GvspSocket set cpu affinity");
//    }

//    s = pthread_getaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
//    if (s != 0) {
//        std::perror("GvspSocket get cpu affinity");
//    }

//    std::clog << "GvspSocket looping on ";
//    for (int j = 0; j < CPU_SETSIZE; j++) {
//        if (CPU_ISSET(j, &cpuset)) {
//            std::clog << "CPU" << j << " ";
//        }
//    }
//    std::clog << std::endl;

//}

struct PACKETRESEND_CMD {
    uint8_t hardKey;
    uint8_t flag;
    uint16_t command;
    uint16_t length;
    uint16_t reqId;
    uint16_t streamChannelIndex;
    uint16_t blockId;
    uint32_t firstPacketId;
    uint32_t lastPacketId;
};

} // anonymous namespace

ReceiverPrivate::ReceiverPrivate(MemoryAllocator *allocator)
    : allocatorPtr(allocator)
{}


struct Jgv::Gvsp::BlockDesc {
    uint32_t version;
    uint32_t userOffset;
    tpacket_hdr_v1 header_v1;
};

void ReceiverPrivate::callResend(uint16_t blockId, uint32_t firstId, uint32_t lastId)
{
    if (sd > 0) {
        PACKETRESEND_CMD resendCmd {
            0x42, 0x00, htons(0x0040),
                    htons(0x000C), htons(0x00FF),
                    htons(0x0000), htons(blockId),
                    htonl(0x00FFFFFF & firstId),
                    htonl(0x00FFFFFF & lastId) };

        sockaddr_in dest = {};
        dest.sin_family = AF_INET;
        dest.sin_port = htons(3956); //GVCP port
        dest.sin_addr.s_addr = htonl(params.transmitterIP);
        socklen_t destlen = sizeof(dest);

        // expédie sans bloquer, pour accélérer les demandes sans ACK (RESENDPACKET en particulier)
        ssize_t sendlen = sendto(sd, &resendCmd, sizeof(resendCmd), MSG_DONTWAIT, reinterpret_cast<const sockaddr *>(&dest), destlen);
        // l'envoi n'est pas passé
        if (sendlen == -1) {
            std::perror("GvcpClient callresend failed");
        }
    }
}

/*!
 * \brief ReceiverPrivate::doResend
 * Applique les règles de demande de réemmission des paquets manquants sur le bloc \a blockId.
 *
 * \param blockId Le numéro de bloc sue lequel les règles doivent être appliquées.
 * \param block Une référence sur le bloc, permettant la mise à jour des statistiques.
 * \param packetId Le numéro de paquet à partir duquel on doit appliquer les règles.
 */
void ReceiverPrivate::doResend(uint16_t blockId, Block &block, uint32_t packetId)
{
    // le nombre que blocs qui précèdent à réemettre
    const auto count = block.emptySegmentsCount(packetId);
    if (count>0 && sd>0) {
        uint32_t firstId = packetId - count;
        uint32_t lastId =  packetId - 1;

        callResend(blockId, firstId, lastId);
        // informe le block
        block.setSegmentsAskedAgain(firstId, lastId);
    }
}


void ReceiverPrivate::userStack()
{
    params.socketType = SocketType::Classic;

    // préparation du polling sur le descripteur de socket
    pollfd pfd {sd,POLLIN,0};
    sockaddr_in from = {};
    socklen_t fromSize = sizeof(from);
    std::vector<uint8_t> buffer(GVSP_BUFFER_SIZE, 0);

    while (run) {
        // attend un évènement à lire
        int pollResult = poll(&pfd, 1, 200);
        if (pollResult > 0) {   // oki, on lit
            const ssize_t read = recvfrom(sd, buffer.data(), GVSP_BUFFER_SIZE, MSG_DONTWAIT, reinterpret_cast<sockaddr *>(&from), &fromSize);
            if (read > 0) {
                if (params.transmitterIP == 0 || ntohl(from.sin_addr.s_addr) == params.transmitterIP) {
                    if (params.transmitterPort == 0 || ntohs(from.sin_port) == params.transmitterPort) {
                        doBuffer(buffer.data(), static_cast<std::size_t>(read));
                    }
                }
            }
            else if (read < 0) {
                std::perror("GvspSocket recv:");
            }
            else {
                std::clog << "GvspSocket recv: 0 byte readed" << std::endl;
            }
        }
    }
}

void ReceiverPrivate::ringStack()
{
    params.socketType = SocketType::RingBuffer;

    raw = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw < 0) {
        std::perror("GvspSocket failed to setup socket");
        return;
    }

    // on utilise la version 3 des socket packet
    tpacket_versions version = TPACKET_V3;
    if (setsockopt(raw, SOL_PACKET, PACKET_VERSION, &version, sizeof(version)) < 0) {
        std::perror("GvspSocket failed to set packet version");
        return;
    }

    // configure le ring
    tpacket_req3 req = {};
    req.tp_block_size = 1 << 21;    // on utilise des blocks de 2 M
    req.tp_frame_size = 1024;       // la taille de la trame est dynamique
    req.tp_block_nr = 16;           // 16 blocs
    req.tp_frame_nr = (req.tp_block_size * req.tp_block_nr) / req.tp_frame_size;
    req.tp_sizeof_priv = 0;
    req.tp_retire_blk_tov = 5;    // timeout 200 ms
    req.tp_feature_req_word = TP_FT_REQ_FILL_RXHASH;
    if (setsockopt(raw, SOL_PACKET, PACKET_RX_RING, &req, sizeof(req)) < 0) {
        std::perror("GvspSocket failed to set packet rx ring");
        return;
    }

    // map le ring
#define CPP_MAP_FAILED	(reinterpret_cast<uint8_t *>( -1 ))
    uint8_t *map = static_cast<uint8_t *>(mmap(nullptr, req.tp_block_size * req.tp_block_nr, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, raw, 0));
    if (map == CPP_MAP_FAILED) {
        std::perror("GvspSocket failed to map ring");
        return;
    }

    // bind le raw socket
    sockaddr_ll localAddress = {};
    localAddress.sll_family   = AF_PACKET;
    localAddress.sll_protocol = htons(ETH_P_IP);
    localAddress.sll_ifindex  = static_cast<int>(nicIndexFromAddress(params.receiverIP));
    localAddress.sll_hatype   = 0;
    localAddress.sll_pkttype  = 0;
    localAddress.sll_halen    = 0;
    if (bind(raw, reinterpret_cast<sockaddr *>(&localAddress), sizeof(localAddress)) == -1) {
        std::perror("GvspSocket failed to bind packet socket");
        return;
    }

    // allocation des descripteurs de block
    std::vector<iovec> vecs;
    vecs.resize(req.tp_block_nr);

    for (uint i=0; i<req.tp_block_nr;++i) {
        vecs[i].iov_base = map + (i * req.tp_block_size);
        vecs[i].iov_len = req.tp_block_size;
    }

    // préparation du polling sur le descripteur de socket
    pollfd pfd = {};
    pfd.fd = raw;
    pfd.events = POLLIN;

    // on commence par le premier block
    unsigned int currentBlockNum = 0;
    while (run) {
        // on caste le bloc
        BlockDesc *bd = reinterpret_cast<BlockDesc *>(vecs[currentBlockNum].iov_base);

        // si le bloc est indisponible
        if ((bd->header_v1.block_status & TP_STATUS_USER) == 0) {
            // on attends un évènement sur le socket packet
            poll(&pfd, 1, 10);
            // le poll est déclenché on recommence la procédure
            continue;
        }
        doBlock(bd);
        bd->header_v1.block_status = TP_STATUS_KERNEL;
        //__sync_synchronize();

        currentBlockNum = (currentBlockNum + 1) % req.tp_block_nr;
    }

    munmap(map, req.tp_block_size * req.tp_block_nr);
    close(raw);
}


void ReceiverPrivate::doBlock(const BlockDesc *block)
{
    // le premier header de type 3 est à l'offset indiqué dans le header tpacket_hdr_v1
    const tpacket3_hdr *header = reinterpret_cast<const tpacket3_hdr*>(reinterpret_cast<const uint8_t *>(block) + block->header_v1.offset_to_first_pkt);

    // on parcourt toutes les trames décrites dans le bloc
    for (unsigned i=0; i<block->header_v1.num_pkts; ++i) {
        // on a besoin de l'ente IP pour connaitre la taille du paquet
        const iphdr *ip = reinterpret_cast<const iphdr *>(reinterpret_cast<const uint8_t *>(header) + header->tp_mac + ETH_HLEN); // on saute l'entete ethernet
        doBuffer(reinterpret_cast<const uint8_t *>(ip) + IP_HEADER_SIZE + UDP_HEADER_SIZE, ntohs(ip->tot_len) - IP_HEADER_SIZE - UDP_HEADER_SIZE);
        // l'offset de l'header suivant est indiqué dans l'header courant
        header = reinterpret_cast<const tpacket3_hdr*>(reinterpret_cast<const uint8_t *>(header) + header->tp_next_offset);
    }
}

void ReceiverPrivate::loop()
{
    // on demande le verrou (bloqué par l'appelant)
    std::unique_lock<std::mutex> lock(mutex);

    // descripteur du socket UDP
    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if ( sd < 0 ) {
        std::perror("GvspSocket: failed to create socket");
    }

    // bind du socket UDP
    sockaddr_in localAddress = {};
    localAddress.sin_family = AF_INET;
    localAddress.sin_port = htons(0);   // port aléatoire
    localAddress.sin_addr.s_addr = htonl(0);


    if ( bind(sd, reinterpret_cast<sockaddr *>(&localAddress), sizeof(struct sockaddr_in) ) == -1 ) {
        std::perror("GvspSocket: failed to bind socket");
    }


    if (params.isMulticast) {
        struct ip_mreq group;
        group.imr_multiaddr.s_addr = htonl(params.multicastGroup);
        group.imr_interface.s_addr = htonl(params.receiverIP);

        if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group)) < 0) {
            std::perror("GvspSocket: failed to set multicast option");
        }
    }

    // on récupère le numéro de port affecté
    struct sockaddr_in bindAddress;
    socklen_t bindAddressSize = sizeof(struct sockaddr_in);
    getsockname(sd, reinterpret_cast<sockaddr *>(&bindAddress), &bindAddressSize);
    params.receiverPort = ntohs(bindAddress.sin_port);

    // on notifie l'appelant que le socket est mode bind et que l'on connait le numéro de port
    cond_variable.notify_all();
    // on lève le verrou
    lock.unlock();

    // fixe l'affinité sur le dernier cpu
    //threadToLastCore();

    // obtient l'ordonnancement temps réel
    setRealtime();

    // si on a la capacity on démarre un socket mmap ring buffer
    if (capng_have_capability(CAPNG_EFFECTIVE, CAP_NET_RAW)) {
        ringStack();
    }
    else {
        userStack();
    }

    close(sd);

#ifdef ENDAT
    if (endatPtr) {
        endatPtr->stop();
    }
#endif
    // purge
    for (auto &it : blocksMap) {
        allocatorPtr->destroy(it.second.image().memory);
    }

    blocksMap.clear();

    std::clog << "GvspSocket closes listener" << std::endl;
}

void ReceiverPrivate::handleLeader(const Packet &gvsp)
{
    statistics.lastTimestamp = updateTimestamp(gvsp);

    auto pair = blocksMap.emplace(std::piecewise_construct, std::forward_as_tuple(gvsp.headerBlockId()), std::forward_as_tuple());

    // si l'insertion est effective, on alloue la mémoire (la clé n'existait pas)
    if (pair.second) {
        pair.first->second.changeGeometry(Geometry {gvsp.leaderImageSizeX(), gvsp.leaderImageSizeY(), gvsp.leaderImagePixelFormat()});
        pair.first->second.image().memory = allocatorPtr->allocate(pair.first->second.image().dataSize);
    }
    // sinon on contrôle que la géometry déjà allouée est la bonne
    else if (pair.first->second.changeGeometry(Geometry {gvsp.leaderImageSizeX(), gvsp.leaderImageSizeY(), gvsp.leaderImagePixelFormat()})) {
        // détruit
        allocatorPtr->destroy(pair.first->second.image().memory);
        // réalloue avec les nouvelles données (le mapping se fera au payload car on ne connait pas le nouveau segment size)
        pair.first->second.image().memory = allocatorPtr->allocate(pair.first->second.image().dataSize);
    }

    // le leader a rempli son rôle
    pair.first->second.setLeaderIsFilled();

    // renseigne les métasdonnées
    pair.first->second.image().timestamp = statistics.lastTimestamp;

#ifdef ENDAT
    if (endatPtr) {
        EndatDatas positions = endatPtr->datas();
        pair.first->second.image().site = positions.firstPosition;
        pair.first->second.image().gisement = positions.secondPosition;
    }
#endif

}

void ReceiverPrivate::handlePayload(const Packet &gvsp)
{
    // si la pile de block est vide, on ne traite pas
    if (blocksMap.size() == 0) {
        return;
    }

    // on a besoin de la dernière géométrie connue
    const Geometry &geometry = blocksMap.crbegin()->second.geometry();

    const auto currentId = gvsp.headerBlockId();
    const auto packetId = gvsp.headerPacketId();


    auto pair = blocksMap.emplace(std::piecewise_construct, std::forward_as_tuple(gvsp.headerBlockId()), std::forward_as_tuple());
    // si l'insertion est effective, on alloue la mémoire
    if (pair.second) {
        pair.first->second.changeGeometry(geometry);
        pair.first->second.image().memory = allocatorPtr->allocate(pair.first->second.image().dataSize);
    }

    // on insert le segment
    pair.first->second.insertSegment(packetId, gvsp.payloadImageData(), gvsp.imageDataSize());

    // on active la politique de resend si activé
    if (params.resend) {
        // on s'occupe du bloc précédent si il existe
        const uint16_t previousId = previousBlockId(currentId);
        if (blocksMap.count(previousId) > 0) {
            Block &previous = blocksMap.at(previousId);
            // c'est depuis le dernier Id qu'on cherche les segments manquants
            doResend(previousId, previous, previous.endID());
        }

        // on s'occupe maintenant du block courant
        // si le leader n'est pas présent on demande un resend
        if (!pair.first->second.leaderIsFilled()) {
            callResend(currentId, 0, 0);
            pair.first->second.setLeaderAskedAgain();
        }
        // on cherche les segments manquants depuis le segment courant
        doResend(currentId, pair.first->second, packetId);
    }
}

void ReceiverPrivate::handleTrailer(const Packet &gvsp)
{
    const auto currentId = gvsp.headerBlockId();
    const auto prevId = previousBlockId(currentId);

    // si le bloc n-1 existe, on l'injecte même s'il n'est pas plein
    if (blocksMap.count(prevId) != 0) {
        Block &block = blocksMap.at(prevId);
        allocatorPtr->push(block.image());

        statistics.imagesCount++;
        if (params.resend) {
            statistics.segmentsLostCount += block.segmentState.empty;
            statistics.segmentsResendCount += block.segmentState.askedAgain;
        }

        blocksMap.erase(prevId);
    }

    // si le bloc n'existe pas, on s'arrête
    // (au niveau du trailer si on n'a toujours rien, c'est peine perdue de tenter d'obtenir les segments manquants)
    if (blocksMap.count(currentId) == 0) {
        return;
    }

    // la référence sur le bloc courant
    Block & block = blocksMap.at(currentId);

    // sauvegarde de caractéristiques du bloc pour la réallocation des suivants
    const auto geometry = block.geometry();
    const auto segmentSize = block.segmentSize();


    // mapSize décrit la taille que devrait avoir la map de blocs
    size_t mapSize = 3;
    // si le block est plein, on le consomme
    if (block.segmentState.empty == 0) {
        // on consomme le bloc
        allocatorPtr->push(block.image());
        statistics.imagesCount++;
        params.geometry = geometry;
        if (params.resend) {
            statistics.segmentsResendCount += block.segmentState.askedAgain;
        }
        blocksMap.erase(currentId);
        // la taille de la map ne sera plus que de 2
        mapSize = 2;
    }
    else if (params.resend) { // si gestion des resends
        doResend(currentId, block, block.endID());
    }



    auto prealloc = [this, &geometry, &segmentSize] (unsigned id) {
        auto next = blocksMap.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple());
        // si l'insertion est effective, on alloue la mémoire et on map
        if (next.second) {
            next.first->second.changeGeometry(geometry);
            next.first->second.image().memory = allocatorPtr->allocate(next.first->second.image().dataSize);
            next.first->second.mapMemory(segmentSize);
        }
    };

    // préalloue les 2 images suivantes
    const uint16_t nextId = nextBlockId(currentId);
    const uint16_t nextNextId = nextBlockId(nextId);
    prealloc(nextId);
    prealloc(nextNextId);


    // on nettoie la map
    auto it = blocksMap.begin();
    while (blocksMap.size() > mapSize) {
        if (it->first == currentId || it->first == nextId || it->first == nextNextId/*(mapSize>2)?it->first==nextNextId:true*/) {
            ++it;
        }
        else {
            allocatorPtr->destroy(it->second.image().memory);
            it = blocksMap.erase(it);
            statistics.imagesLostCount++;
        }
    }
}

uint64_t ReceiverPrivate::updateTimestamp(const Packet &gvsp)
{
    if (clock.src == TimestampSource::GEVTransmitter) {
        uint64_t t = timestamp(gvsp.leaderTimestampHigh(), gvsp.leaderTimestampLow());
        // applique le facteur de base de temps si l'horloge n'est pas à 1 GHz
        if (params.timestampFrequency != 1000000000) {
            t *= static_cast<double>(1000000000./params.timestampFrequency);
        }
        // le temps caméra est en TAI
        return t - clock.UtcOffset;
    }
    if (clock.src == TimestampSource::TimestampDate) {
        return datation.getDate(timestamp(gvsp.leaderTimestampHigh(), gvsp.leaderTimestampLow()));
    }

    timespec start;
    if (clock_gettime(CLOCK_REALTIME, &start) >= 0) {
        return static_cast<uint64_t>(start.tv_sec) * UINT64_C(1000000000) + static_cast<uint64_t>(start.tv_nsec);
    }

    return 0;
}

void ReceiverPrivate::doBuffer(const uint8_t *buffer, std::size_t size)
{
    Packet gvsp(buffer, size);
    const uint16_t status = gvsp.headerStatus();
    if  (status == enumType(Status::Succes)) {
        switch (gvsp.headerPacketFormat()) {
        case enumType(PacketFormat::DATA_PAYLOAD):
            handlePayload(gvsp);
            break;
        case enumType(PacketFormat::DATA_LEADER):
            handleLeader(gvsp);
            break;
        case enumType(PacketFormat::DATA_TRAILER):
            handleTrailer(gvsp);
            break;
        default:
            std::clog << "GvspSocket: packet format not handled" << std::endl;
        }
    }

    else if (status == enumType(Status::Resend)) {
        //blockPtr->insertSegment(packetID, gvsp.imageData(), gvsp.imageDataSize());
        std::clog << "GvspSocket: resend" << std::endl;

    }

    else if (status == 0x4100) {
        // il s'agit apparement d'un bug PLEORA, devrait être 0x0100 (STATUS_PACKET_RESEND)
        //blockPtr->insertSegment(packetID, gvsp.imageData(), gvsp.imageDataSize());
    }

    else {
        // std::clog << "GvspSocket: unsucces " << Status::toString(status) << std::endl;
    }
}

Receiver::Receiver()
    : p_impl(new ReceiverPrivate(new MemoryAllocator))
{}

Receiver::Receiver(MemoryAllocator *allocator)
    : p_impl(new ReceiverPrivate(allocator))
{}

Receiver::Receiver(ReceiverPrivate &dd)
    : p_impl(&dd)
{}

Receiver::~Receiver()
{
    IMPL(Receiver);
    if (d->threadPtr && d->threadPtr->joinable()) {
        d->run = false;
        d->threadPtr->join();
    }
}

/*!
 * \brief Receiver::listenOn
 * Place le receveur en écoute.
 * \param bindAddress L'addresse IPV4 utilisée par le receveur.
 */
void Receiver::listenUnicast(uint32_t bindAddress)
{
    IMPL(Receiver);
    if (d->threadPtr) {
        std::clog << "Receiver allready listening" << std::endl;
        return;
    }

    // on bloque le mutex
    std::unique_lock<std::mutex> lock(d->mutex);

    // l'ip du receveur
    d->params.receiverIP = bindAddress;

    // on lance le thread
    d->threadPtr = std::unique_ptr<std::thread>(std::move(new std::thread(&ReceiverPrivate::loop, d)));
    pthread_setname_np(d->threadPtr->native_handle(), "GvspSocket");

    // on débloque le mutex et on attends
    d->cond_variable.wait(lock);
}

void Receiver::listenMulticast(uint32_t bindAddress, uint32_t multicastAddress)
{
    IMPL(Receiver);

    d->params.multicastGroup = multicastAddress;
    d->params.isMulticast = true;
    listenUnicast(bindAddress);

    std::clog << ("Listen multicast");

}

void Receiver::acceptFrom(uint32_t transmitterIP, uint16_t transmitterPort)
{
    IMPL(Receiver);

    d->params.transmitterIP = transmitterIP;
    d->params.transmitterPort = transmitterPort;

    if (d->params.socketType == SocketType::RingBuffer) {
        setFilter(d->raw, d->params.transmitterIP, d->params.transmitterPort, d->params.receiverIP, d->params.receiverPort);
    }
}


constexpr const std::size_t HEADER_SIZE = IP_HEADER_SIZE + UDP_HEADER_SIZE + GVSP_HEADER_SIZE;
void Receiver::preallocImages(const Geometry &geometry, uint32_t packetSize)
{
    IMPL(Receiver);

    if (packetSize < HEADER_SIZE) {
        std::clog << "GvspReceiver failed to prealloc images: bad packetSize" << std::endl;
        return;
    }


    const std::size_t segmentSize = packetSize - HEADER_SIZE;
    auto insert = [d, &geometry, segmentSize] (unsigned id) {
        auto pair = d->blocksMap.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple());
        // si l'insertion est effective, on alloue la mémoire et on la map
        if (pair.second) {
            pair.first->second.changeGeometry(geometry);
            pair.first->second.image().memory = d->allocatorPtr->allocate(pair.first->second.image().dataSize);
            pair.first->second.mapMemory(segmentSize);
        }
    };

    insert(1);
    insert(2);

    d->params.geometry = geometry;

}

void Receiver::setTimestampSource(TimestampSource source)
{
    IMPL(Receiver);
    d->clock.src = source;
    d->params.timestampSrc = source;
}

void Receiver::setTransmitterTimestampFrequency(uint64_t frequency)
{
    IMPL(Receiver);

    // on bloque le mutex
    std::unique_lock<std::mutex> lock(d->mutex);
    d->params.timestampFrequency = frequency;
}

void Receiver::setResendActive(bool active)
{
    IMPL(Receiver);
    d->params.resend = active;
    d->statistics.segmentsLostCount = 0;
    d->statistics.segmentsResendCount = 0;
}

void Receiver::pushDatation(uint64_t timestamp, uint64_t dateMin, uint64_t dateMax)
{
    IMPL(Receiver);

    d->datation.push(timestamp, dateMin, dateMax);
}

const Parameters &Receiver::parameters() const
{
    IMPL(const Receiver);
    return d->params;
}

const ReceiverStatistics &Receiver::statistics() const
{
    IMPL(const Receiver);
    return d->statistics;
}

MemoryAllocator &Receiver::allocator()
{
    IMPL(Receiver);
    return *d->allocatorPtr.get();
}

const MemoryAllocator &Receiver::allocator() const
{
    IMPL(const Receiver);
    return *d->allocatorPtr.get();
}

#ifdef ENDAT
std::weak_ptr<Endat> Receiver::getEndat()
{
    IMPL(const Receiver);
    return d->endatPtr;
}
#endif
