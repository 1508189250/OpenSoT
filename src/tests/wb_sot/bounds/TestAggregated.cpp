#include <gtest/gtest.h>
#include <wb_sot/bounds/Aggregated.h>
#include <wb_sot/bounds/velocity/VelocityLimits.h>
#include <wb_sot/bounds/BilateralConstraint.h>
#include <wb_sot/bounds/velocity/JointLimits.h>
#include <string>

namespace {

// The fixture for testing class Foo.
class testAggregated : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  testAggregated() {
    // You can do set-up work for each test here.
  }

  virtual ~testAggregated() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(testAggregated, AggregatedWorks) {
    using namespace wb_sot::bounds;
    std::list<Aggregated::BoundPointer> constraints;
    const unsigned int nJ = 6;
    const double dT = 0.1;
    const double qDotMax = 0.5;
    constraints.push_back(  Aggregated::BoundPointer(
            new velocity::VelocityLimits(qDotMax,dT,nJ)
                                                    )
                          );

    yarp::sig::Vector q(nJ, 0.0);
    yarp::sig::Vector q_next(nJ, M_PI - 0.01);

    yarp::sig::Matrix A(nJ,nJ); A.eye();
    yarp::sig::Vector bUpperBound(nJ,M_PI);
    yarp::sig::Vector bLowerBound(nJ,0.0);
    constraints.push_back(Aggregated::BoundPointer(
        new BilateralConstraint(A, bUpperBound, bLowerBound)
                                                  )
                          );

    constraints.push_back(Aggregated::BoundPointer(
        new velocity::JointLimits(q, bUpperBound, bLowerBound)
                                                  )
                          );
    Aggregated::BoundPointer aggregated(new Aggregated(constraints, q));

    /* we should mash joint limits and velocity limits in one */
    EXPECT_TRUE(aggregated->getLowerBound().size() == nJ);
    EXPECT_TRUE(aggregated->getUpperBound().size() == nJ);
    /* we have a BilateralConstraint... */
    EXPECT_TRUE(aggregated->getAineq().rows() == nJ);
    EXPECT_TRUE(aggregated->getbLowerBound().size() == nJ);
    EXPECT_TRUE(aggregated->getbUpperBound().size() == nJ);
    /* and no equality constraint */
    EXPECT_TRUE(aggregated->getAeq().rows() == 0);
    EXPECT_TRUE(aggregated->getbeq().size() == 0);

    yarp::sig::Vector oldLowerBound = aggregated->getLowerBound();
    yarp::sig::Vector oldUpperBound = aggregated->getUpperBound();
    aggregated->update(q_next);
    yarp::sig::Vector newLowerBound = aggregated->getLowerBound();
    yarp::sig::Vector newUpperBound = aggregated->getUpperBound();
    EXPECT_FALSE(oldLowerBound == newLowerBound);
    EXPECT_FALSE(oldUpperBound == newUpperBound);
}


}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}