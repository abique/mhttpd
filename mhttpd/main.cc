#include <mimosa/init.hh>
#include <mimosa/log/log.hh>
#include <mimosa/options/options.hh>
#include <mimosa/http/server.hh>
#include <mimosa/http/dispatch-handler.hh>
#include <mimosa/http/fs-handler.hh>
#include <mimosa/http/log-handler.hh>

uint16_t & PORT = *mimosa::options::addOption<uint16_t>("", "port", "the port to use", 4242);
std::string & PATH = *mimosa::options::addOption<std::string>("", "path", "the data dir to expose", "/usr");
std::string & CERT = *mimosa::options::addOption<std::string>("", "cert", "the certificate (cert.pem)", "");
std::string & KEY = *mimosa::options::addOption<std::string>("", "key", "the key (key.pem)", "");
uint64_t & TIMEOUT = *mimosa::options::addOption<uint64_t>("", "timeout", "the read & write timeout in ms", 5000);

int main(int argc, char **argv)
{
  mimosa::init(argc, argv);

  {
    auto dispatch(new mimosa::http::DispatchHandler);
    dispatch->registerHandler("/*", new mimosa::http::FsHandler(PATH, 0, true));

    auto logger(new mimosa::http::LogHandler);
    logger->setHandler(dispatch);

    mimosa::http::Server::Ptr server(new mimosa::http::Server);
    server->setReadTimeout(TIMEOUT * mimosa::millisecond);
    server->setWriteTimeout(TIMEOUT * mimosa::millisecond);
    server->setHandler(logger);

    if (!CERT.empty() && !KEY.empty())
      server->setSecure(CERT, KEY);

    auto ret = server->listenInet4(PORT);
    if (!ret) {
      mimosa::log::error("failed to listen");
      return 1;
    }
    mimosa::log::info("listen on %d succeed\n", PORT);

    while (true)
      server->serveOne();
  }

  mimosa::deinit();

  return 0;
}
