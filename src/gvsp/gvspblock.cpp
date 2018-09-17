#include "gvspblock.h"
#include "gvsp.h"

#include <cstring>
#include <iostream>

using namespace Jgv::Gvsp;

struct Jgv::Gvsp::PtrInfos
{
    uint8_t *ptr = nullptr;
    std::size_t size = 0;
    SegmentState state = SegmentState::empty;
};

Block::Block()
#ifdef ENDAT
    : m_image { {0,0,0}, 0, 0, 0, 0, 0, {}}
    #else
    : m_image { {0,0,0}, 0, 0, 0, {}}
    #endif
{}

Block::~Block()
{}

/*!
 * \brief Block::image
 * Fournit une référence modifiable sur les données d'image.
 *
 * \return Une référence modifiable sur Jgv::Gvsp::Image
 */
Image &Block::image() noexcept
{
    return m_image;
}

/*!
 * \brief Block::image
 * Fournit une référence constante sur les données d'image.
 *
 * \return Une référence non modifiable sur Jgv::Gvsp::Image
 */
const Image & Block::image() const noexcept
{
    return m_image;
}

/*!
 * \brief Block::geometry
 * Fournit une référence constante sur les données de géomtrie de l'image.
 * \return Une référence non modifiable sur Jgv::Gvsp::Geometry
 */
const Geometry &Block::geometry() const noexcept
{
    return m_image.geometry;
}

/*!
 * \brief Block::segmentSize
 * Obtient la taille du segment d'image courant.
 * \return La taille du segment image.
 */
std::size_t Block::segmentSize() const noexcept
{
    return (m_memoryMapping.empty()) ? 0 : m_memoryMapping.front().size;
}

/*!
 * \brief GvspBlock::endID
 * \return Le numéro de segment suivant le dernier numéro de segment valide.
 */
uint32_t Block::endID() const noexcept
{
    return static_cast<uint32_t>(m_memoryMapping.size()) + 1;
}

/*!
 * \brief Block::changeGeometry
 * Change la géométrie de l'image portée par le bloc.
 * Si le changement est effectif, invalide la cartographie mémoire.
 *
 * \param geometry La nouvelle géométrie de l'image.
 * \return Vrai si la géométrie a effectivement été changé.
 */
bool Block::changeGeometry(const Geometry &geometry) noexcept
{
    if (m_image.geometry != geometry) {
        const unsigned pixelsCount = (geometry.width * geometry.height);
        const unsigned pixelSize = GVSP_PIX_PIXEL_SIZE(geometry.pixelFormat);
        const unsigned alignedPixelSize = pixelSize + 8 - (pixelSize % 8);
        m_image.geometry = geometry;
        m_image.geometrySize = (pixelsCount * pixelSize) / 8;
        m_image.dataSize = (pixelsCount * alignedPixelSize) / 8;
        m_memoryMapping.clear();
        return true;
    }
    return false;
}

/*!
 * \brief Block::mapMemory
 * Construit la cartographie de la mémoire allouée à l'image.
 * Chaque index de segment se voit affecter un pointeur sur la mémoire image.
 *
 * \param segmentSize La taille de chaque segment de mémoire.
 */
void Block::mapMemory(std::size_t segmentSize) noexcept
{
    if (segmentSize == 0) {
        std::cerr << "GvspBlock failed to map memory, segmentSize = 0" << std::endl;
        return;
    }

    if (m_image.memory.data == nullptr) {
        std::cerr << "GvspBlock failed to map memory on nullptr memory" << std::endl;
        return;
    }

    // calcul de la taille du dernier segment
    const std::size_t lastSize = m_image.geometrySize % segmentSize;

    // calcul du nombre de segments
    const std::vector<PtrInfos>::size_type segmentsCount = (lastSize==0) ? m_image.geometrySize/segmentSize : 1 + m_image.geometrySize/segmentSize;

    // contruit la cartographie
    m_memoryMapping.resize(segmentsCount);

    uint8_t *p = m_image.memory.data;
    for (auto &ptrInfos: m_memoryMapping) {
        ptrInfos.ptr = p;
        ptrInfos.size = segmentSize;
        p += segmentSize;
    }

    // on corrige la taille du dernier segment
    m_memoryMapping.back().size = lastSize==0 ? segmentSize : lastSize;

    // met à jour les statistiques
    segmentState.empty = segmentsCount;
    segmentState.askedAgain = 0;
}

void Block::insertSegment(unsigned packetId, const uint8_t * const segment, std::size_t segmentSize) noexcept
{
    // rejette les segments hors plage
    if (packetId == 0) {
        std::clog << "GvspBlock cant insert packet Id 0" << std::endl;
        return;
    }

    // l'initialisation n'a pas encore eu lieu, on map la mémoire
    if (m_memoryMapping.size() == 0) {
        mapMemory(segmentSize);
        if (m_memoryMapping.empty()) {
            std::cerr << "GvspBlock can't insert segment on empty memory map" << std::endl;
            return;
        }
    }

    // si le numéro de segment est en dehors de la plage, on rejette
    if (packetId > m_memoryMapping.size()) {
        std::cerr << "GvspBlock: segnum out of range(1, " << m_memoryMapping.size() << ") " << packetId << std::endl;
        return;
    }

    PtrInfos &ptrInfos = m_memoryMapping[packetId - 1];

    // on rejette les segments dont la taille ne corresponds pas à la carte calculée
    if (segmentSize != ptrInfos.size) {
        // si on a affaire au dernier segment, il est possible que la caméra fait du bourrage,
        // du coup il peut être égal à la taille des autres segments
        if (segmentSize != m_memoryMapping.front().size) {
            std::cerr << "GvspBlock: segmentSize != mapped size " << segmentSize << " " << ptrInfos.size << std::endl;
            return;
        }
    }

    // on copie seulement si cela n'a pas déjà été fait.
    if (ptrInfos.state != SegmentState::filled) {
        std::memcpy(ptrInfos.ptr, segment, ptrInfos.size);
        ptrInfos.state = SegmentState::filled;
        // mets à jour les statistiques
        segmentState.empty--;
    }
}

void Block::setLeaderIsFilled() noexcept
{
    m_leaderState = SegmentState::filled;
}

bool Block::leaderIsFilled() const noexcept
{
    return m_leaderState == SegmentState::filled;
}

void Block::setLeaderAskedAgain() noexcept
{
    m_leaderState = SegmentState::askedAgain;
}

void Block::setSegmentsAskedAgain(std::size_t startID, std::size_t stopID) noexcept
{

    // on convertit les numeros de segments en index
    for (std::size_t index = startID - 1; index <= stopID -1; ++index) {
        m_memoryMapping[index].state = SegmentState::askedAgain;
        // met à jourles statistiques
        segmentState.askedAgain++;
    }
}

unsigned Block::emptySegmentsCount(unsigned packetId) const noexcept
{
    // index du segment précédent packedId
    int index = static_cast<int>(packetId) - 2;

    if (index < 0) {
        return 0;
    }

    unsigned count = 0;

    for (; index >= 0; --index) {
        if (m_memoryMapping[static_cast<std::vector<PtrInfos>::size_type>(index)].state == SegmentState::empty)
            count++;
        else break;
    }
    return count;
}


//void Block::dump() const
//{
//    std::clog << "# DUMP #" << std::endl;
//    for (int i=0; i<m_memoryMapping.size(); ++i) {
//        std::clog << i << " " << (int)m_memoryMapping[i].state << std::endl;
//    }
//    std::clog << "########" << std::endl;
//}
