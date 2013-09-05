// enrichment_facility_tests.cc
#include <gtest/gtest.h>

#include "enrichment_facility_tests.h"

#include "facility_model_tests.h"
#include "model_tests.h"

#include "commodity.h"
#include "xml_query_engine.h"

#include <sstream>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentFacilityTest::SetUp() {
  cyclus::Context* ctx = tc_.get();
  src_facility = new cycamore::EnrichmentFacility(ctx);

  InitParameters();
  InitFacility();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentFacilityTest::TearDown() {
  delete src_facility;
  delete out_commod_market;
  delete in_commod_market;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentFacilityTest::InitParameters() {
  cyclus::Context* ctx = tc_.get();
  
  in_commod = "incommod";
  in_commod_market = new TestMarket(ctx);
  in_commod_market->SetCommodity(in_commod);
  cyclus::MarketModel::RegisterMarket(in_commod_market);

  out_commod = "outcommod";
  out_commod_market = new TestMarket(ctx);
  out_commod_market->SetCommodity(out_commod);
  cyclus::MarketModel::RegisterMarket(out_commod_market);

  in_recipe = "recipe";
  feed_assay = 0.0072;

  cyclus::CompMap v;
  v[92235] = feed_assay;
  v[92238] = 1 - feed_assay;
  recipe = cyclus::Composition::CreateFromAtom(v);
  ctx->RegisterRecipe(in_recipe, recipe);

  tails_assay = 0.002;
  inv_size = 5;
  commodity_price = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentFacilityTest::InitFacility() {
  std::stringstream ss("");
  ss << "<start>"
     << "  <input>"
     << "    <incommodity>" << in_commod << "</incommodity>"
     << "    <inrecipe>" << in_recipe << "</inrecipe>"
     << "    <inventorysize>" << inv_size << "</inventorysize>"
     << "  </input>"
     << "  <output>"
     << "    <outcommodity>" << out_commod << "</outcommodity>"
     << "    <tails_assay>" << tails_assay << "</tails_assay>"
     << "  </output>"
     << "</start>";

  cyclus::XMLParser parser;
  parser.Init(ss);
  cyclus::XMLQueryEngine* engine = new cyclus::XMLQueryEngine(parser);

  EXPECT_NO_THROW(src_facility->InitModuleMembers(engine));
  delete engine;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentFacilityTest, init) {
  EXPECT_EQ(in_recipe, src_facility->in_recipe());
  EXPECT_EQ(in_commod, src_facility->in_commodity());
  EXPECT_EQ(out_commod, src_facility->out_commodity());
  EXPECT_DOUBLE_EQ(tails_assay, src_facility->tails_assay());
  EXPECT_DOUBLE_EQ(feed_assay, src_facility->feed_assay());
  EXPECT_DOUBLE_EQ(inv_size, src_facility->MaxInventorySize());
  EXPECT_DOUBLE_EQ(commodity_price, src_facility->commodity_price());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentFacilityTest, clone) {
  cyclus::Context* ctx = tc_.get();
  
  cycamore::EnrichmentFacility* cloned_fac =
      new cycamore::EnrichmentFacility(ctx);
  cloned_fac->CloneModuleMembersFrom(src_facility);

  EXPECT_EQ(in_recipe, cloned_fac->in_recipe());
  EXPECT_EQ(in_commod, cloned_fac->in_commodity());
  EXPECT_EQ(out_commod, cloned_fac->out_commodity());
  EXPECT_DOUBLE_EQ(tails_assay, cloned_fac->tails_assay());
  EXPECT_DOUBLE_EQ(feed_assay, cloned_fac->feed_assay());
  EXPECT_DOUBLE_EQ(inv_size, cloned_fac->MaxInventorySize());
  EXPECT_DOUBLE_EQ(commodity_price, cloned_fac->commodity_price());

  delete cloned_fac;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* EnrichmentFacilityModelConstructor(cyclus::Context* ctx) {
  using cycamore::EnrichmentFacility;
  return dynamic_cast<cyclus::Model*>(new EnrichmentFacility(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::FacilityModel* EnrichmentFacilityConstructor(cyclus::Context* ctx) {
  using cycamore::EnrichmentFacility;
  return dynamic_cast<cyclus::FacilityModel*>(new EnrichmentFacility(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(EnrichmentFac, FacilityModelTests,
                        Values(&EnrichmentFacilityConstructor));
INSTANTIATE_TEST_CASE_P(EnrichmentFac, ModelTests,
                        Values(&EnrichmentFacilityModelConstructor));