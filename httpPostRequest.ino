

HttpPostRequest::HttpPostRequest(EthernetClient *c,char *d){
    client = c;
    data = d;

    // calculate lenght of data
    content_length = String(d).length();
}

int HttpPostRequest::send(byte *server_address, int port){
    if(client->connect( server_address , port )) {
      client->println("POST /api/v1/users/ HTTP 1.0");
      // client->print("Host: ");
      // client->print(*server_address);
      // client->print(":");
      // client->println(port);
      client->println("User-Agent: Arduino");
      client->println("Accept: application/json");
      client->println("Content-Type: application/json");
      client->print("Content-Length: ");
      client->println(content_length);
      client->println();
      client->println(data);
      client->stop();
    }
    return 0;
}
  // enable this after debuging is done
  // private:

  // int content_length;
