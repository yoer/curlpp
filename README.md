# curlimpp
simple curl encapsulation by cplusplus

implement some normal http methods by curl library, like get/post/download

###Sample
    curlpp::hxxp().globalInit();
    curlpp::net_default_data default_data;
    default_data.download_path(".");
    default_data.timeout(10);
    
    curlpp::result get_rs = 
        curlpp::hxxp()
        .url("http://www.sample.com")
        .get();
    if (get_rs.state()){
      std::cout << "get success" << std::endl;
    }else{
      std::cout << "get fail," << get_rs.value() << std::endl;
    }
    
    curlpp::result post_rs = 
        curlpp::hxxp()
        .url("http://www.sample.com")
        .add_header("")
        .add_header("")
        .add_post_params("", "")
        .post();
    if (get_rs.state()){
      std::cout << "post success" << std::endl;
    }else{
      std::cout << "post fail," << get_rs.value() << std::endl;
    }
    
    curlpp::result download_rs =
        curlpp::hxxp()
        .url("http://www.sample.com/img/sample.png")
        .download_path(".")
        .outfile_name("save.png")
        .download();
    if (download_rs.state()){
      std::cout << "download success" << std::endl;
    }else{
      std::cout << "download fail," << download_rs.value() << std::endl;
    }
    
    curlpp::hxxp().globalClean();
