#include "envoy/config/filter/network/ext_authz/v2/ext_authz.pb.h"

#include "extensions/filters/network/ext_authz/config.h"

#include "test/mocks/server/mocks.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::_;
using testing::Invoke;

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace ExtAuthz {

TEST(ExtAuthzFilterConfigTest, ValidateFail) {
  NiceMock<Server::Configuration::MockFactoryContext> context;
  EXPECT_THROW(ExtAuthzConfigFactory().createFilterFactoryFromProto(
                   envoy::config::filter::network::ext_authz::v2::ExtAuthz(), context),
               ProtoValidationException);
}

TEST(ExtAuthzFilterConfigTest, ExtAuthzCorrectProto) {
  std::string yaml = R"EOF(
  grpc_service:
    google_grpc:
      target_uri: ext_authz_server
      stat_prefix: google
  failure_mode_allow: false
  stat_prefix: name
)EOF";

  ExtAuthzConfigFactory factory;
  ProtobufTypes::MessagePtr proto_config = factory.createEmptyConfigProto();
  MessageUtil::loadFromYaml(yaml, *proto_config);

  NiceMock<Server::Configuration::MockFactoryContext> context;

  EXPECT_CALL(context.cluster_manager_.async_client_manager_, factoryForGrpcService(_, _, _))
      .WillOnce(Invoke([](const envoy::api::v2::core::GrpcService&, Stats::Scope&, bool) {
        return std::make_unique<NiceMock<Grpc::MockAsyncClientFactory>>();
      }));
  Network::FilterFactoryCb cb = factory.createFilterFactoryFromProto(*proto_config, context);
  Network::MockConnection connection;
  EXPECT_CALL(connection, addReadFilter(_));
  cb(connection);
}

} // namespace ExtAuthz
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
