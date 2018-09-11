#include <fstream>
#include <sstream>
#include <genicam/nodemap.h>

#include <Poco/CppUnit/TestRunner.h>
#include <Poco/CppUnit/CppUnit.h>
#include <Poco/CppUnit/TestCase.h>
#include <Poco/CppUnit/TestSuite.h>
#include <Poco/CppUnit/TestCaller.h>

class GenICamXMLTest : public CppUnit::TestCase
{
public:
    GenICamXMLTest(const std::string &name) : CppUnit::TestCase(name) {}
    ~GenICamXMLTest() = default;
    static CppUnit::Test* suite() {
        CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("GenICamXMLTest");

        CppUnit_addTest(pSuite, GenICamXMLTest, testXMLParse);

        return pSuite;
    }

    void testXMLParse() {
        std::fstream fs("XCG-CSeries_v_1_1_0.xml");
        std::stringstream ss;
        ss << fs.rdbuf();

        std::shared_ptr<Jgv::GenICam::GenICamXMLParser> xmlParser
            = std::make_shared<Jgv::GenICam::GenICamXMLParser>();
        xmlParser->parse(ss.str());

        std::shared_ptr<Jgv::GenICam::NodeMap> nodeMap =
            std::make_shared<Jgv::GenICam::NodeMap>(xmlParser,
                      std::shared_ptr<Jgv::GenICam::IPort::Interface>(0));

        std::vector<std::string> categories = nodeMap->categoryNames();
        for (auto &p : categories) {
            std::cout << "Category: " << std::endl;
            std::vector<std::string> features = nodeMap->featureNames(p);
            for (auto &f : features) {
                std::cout << f << std::endl;
            }
        }
    }
};

class GenICamTestSuite
{
public:
    static CppUnit::Test *suite() {
        CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("GenICamTestSuite");

        pSuite->addTest(GenICamXMLTest::suite());

        return pSuite;
    }
};


CppUnitMain(GenICamTestSuite)
