//Copyright 2021 pan_fera
#include "header.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
  std::shared_ptr<std::shared_mutex> mutex =
      std::make_shared<std::shared_mutex>();
  std::shared_ptr<JsonArray> arr =
      std::make_shared<JsonArray>("../misc/suggestions.json");
  std::shared_ptr<Suggestions> suggestions = std::make_shared<Suggestions>();

  try {
    if (argc != 3) {
      std::cerr << "Usage: http-server-sync <address> <port> \n"
                << "Example:\n"
                << "    http-server-sync 0.0.0.0 8080\n";
      return EXIT_FAILURE;
    }
    std::thread{update_suggestions, mutex, arr, suggestions}.detach();

    auto const address = net::ip::make_address(argv[1]);
    auto const port = static_cast<uint16_t>(std::atoi(argv[2]));

    net::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {address, port}};
    for (;;) {
      tcp::socket socket{ioc};
      acceptor.accept(socket);
      std::thread{std::bind(&do_session, std::move(socket), suggestions, mutex)}
          .detach();
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
