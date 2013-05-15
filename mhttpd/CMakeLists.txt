add_executable(mhttpd
  main.cc)

target_link_libraries(mhttpd mimosa-http mimosa)

install(TARGETS mhttpd
  RUNTIME DESTINATION bin)
