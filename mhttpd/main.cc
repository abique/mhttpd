#include <mimosa/init.hh>
#include <mimosa/priviledge-drop.hh>
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
uint64_t & TIMEOUT = *mimosa::options::addOption<uint64_t>("", "timeout", "the read & write timeout in seconds", 0);

bool & DISABLE_GET = *mimosa::options::addSwitch("", "disable-get", "disable get");
bool & DISABLE_HEAD = *mimosa::options::addSwitch("", "disable-head", "disable head");
bool & ENABLE_READDIR = *mimosa::options::addSwitch("", "enable-readdir", "enables directory listing");
bool & ENABLE_PUT = *mimosa::options::addSwitch("", "enable-put", "enables put");
bool & ENABLE_DELETE = *mimosa::options::addSwitch("", "enable-delete", "enables delete");
bool & ENABLE_XATTR = *mimosa::options::addSwitch("", "enable-xattr", "store/fetch content type in xattr");
bool & ENABLE_MKCOL = *mimosa::options::addSwitch("", "enable-mkcol", "enables mkcol");
bool & ENABLE_MOVE = *mimosa::options::addSwitch("", "enable-move", "enables move");

int main(int argc, char **argv)
{
  mimosa::init(argc, argv);
  mimosa::priviledgeDrop();

  {
    auto dispatch(new mimosa::http::DispatchHandler);
    auto fs_handler = new mimosa::http::FsHandler(PATH, 0);
    fs_handler->enableHead(!DISABLE_GET);
    fs_handler->enableGet(!DISABLE_HEAD);
    fs_handler->enableReaddir(ENABLE_READDIR);
    fs_handler->enablePut(ENABLE_PUT);
    fs_handler->enableDelete(ENABLE_DELETE);
    fs_handler->enableMkcol(ENABLE_MKCOL);
    fs_handler->enableXattr(ENABLE_XATTR);
    fs_handler->enableMove(ENABLE_MOVE);
    dispatch->registerHandler("/*", fs_handler);

    auto logger(new mimosa::http::LogHandler);
    logger->setHandler(dispatch);

    mimosa::http::Server::Ptr server(new mimosa::http::Server);
    server->setReadTimeout(TIMEOUT * mimosa::second);
    server->setWriteTimeout(TIMEOUT * mimosa::second);
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
