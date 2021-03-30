//Copyright 2021 pan_fera
#include "header.hpp"

template <class Body, class Allocator, class Send>
void handle_request(http::request<Body, http::basic_fields<Allocator>>&& req,
                    Send&& send,
                    const std::shared_ptr<Suggestions>& suggestions,
                    const std::shared_ptr<std::shared_mutex>& mutex) {
  [[maybe_unused]] auto const bad_request = [&req](beast::string_view why) {
    http::response<http::string_body> res{http::status::bad_request,
                                          req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
  };

  [[maybe_unused]] auto const server_answer = [&req](const json& what) {
    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    std::stringstream help;
    help << what << std::endl;
    res.body() = help.str();
    res.prepare_payload();
    return res;
  };
  [[maybe_unused]] auto const not_found = [&req](beast::string_view target) {
    http::response<http::string_body> res{http::status::not_found,
                                          req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "The resource '" + std::string(target) + "' was not found.";
    res.prepare_payload();
    return res;
  };

  if (req.target() != "/v1/api/suggest") return send(not_found(req.target()));

  std::regex rx(R"(\{"input":"[\w\s]*"})");
  if (!regex_match(req.body().begin(), req.body().end(), rx))
    return send(bad_request("Illegal request"));

  json input = json::parse(req.body())["input"];
  mutex->lock_shared();
  auto result = suggestions->DoSuggest(input);
  mutex->unlock_shared();
  std::cout << result.dump(4) << std::endl;
  return send(server_answer(result));
}

void fail(beast::error_code ec, char const* what) {
  std::cerr << what << ": " << ec.message() << ec.category().name() << "\n";
}

void do_session(tcp::socket& socket,
                const std::shared_ptr<Suggestions>& suggestions,
                const std::shared_ptr<std::shared_mutex>& mutex) {
  bool close = false;
  beast::error_code ec;
  beast::flat_buffer buffer;
  send_lambda<tcp::socket> lambda{socket, close, ec};

  for (;;) {
    http::request<http::string_body> req;
    http::read(socket, buffer, req, ec);
    if (ec == http::error::end_of_stream) break;
    if (ec) return fail(ec, "read");
    std::cout << req.body() << std::endl;
    handle_request(std::move(req), lambda, suggestions, mutex);
    if (ec) return fail(ec, "write");
    if (close) {
      break;
    }
  }

  socket.shutdown(tcp::socket::shutdown_send, ec);
}

[[noreturn]] void update_suggestions(
    const std::shared_ptr<std::shared_mutex>& mutex,
    const std::shared_ptr<JsonArray>& arr,
    const std::shared_ptr<Suggestions>& suggestion) {
  for (;;) {
    mutex->lock();
    arr->ReadJson();
    suggestion->Update(arr->GetMemory());
    mutex->unlock();
    std::this_thread::sleep_for(std::chrono::minutes(15));
  }
}
