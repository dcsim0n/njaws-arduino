

HttpPostRequest::HttpPostRequest(EthernetClient c,char *d){
    client = c;
    data = d;

    // calculate lenght of data
    String dataStr = String(data);
    content_length = dataStr.length();
}

int HttpPostRequest::send(){
    return 0;
}
  // enable this after debuging is done
  // private:

  // int content_length;
