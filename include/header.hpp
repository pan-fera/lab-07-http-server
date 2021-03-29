//
// Copyright 2021 pan_fera
//

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <regex>
#include <shared_mutex>
#include <string>
#include <thread>

#include "JsonArray.hpp"
#include "Suggestions.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;


template <class Stream>
struct send_lambda {
  Stream& stream_;
  bool& close_;
  beast::error_code& ec_;

  explicit send_lambda(Stream& stream, bool& close, beast::error_code& ec)
      : stream_(stream), close_(close), ec_(ec) {}

  template <bool isRequest, class Body, class Fields>
  void operator()(http::message<isRequest, Body, Fields>&& msg) const {
    close_ = msg.need_eof();
    http::serializer<isRequest, Body, Fields> sr{msg};
    http::write(stream_, sr, ec_);
  }
};

template <class Body, class Allocator, class Send>
void handle_request(http::request<Body, http::basic_fields<Allocator>>&& req,
                    Send&& send,
                    const std::shared_ptr<Suggestions>& suggestions,
                    const std::shared_ptr<std::shared_mutex>& mutex);

void fail(beast::error_code ec, char const* what);

void do_session(tcp::socket& socket,
                const std::shared_ptr<Suggestions>& suggestions,
                const std::shared_ptr<std::shared_mutex>& mutex);

[[noreturn]] void update_suggestions(
    const std::shared_ptr<std::shared_mutex>& mutex,
    const std::shared_ptr<JsonArray>& arr,
    const std::shared_ptr<Suggestions>& suggestion);


#endif  // INCLUDE_HEADER_HPP_
