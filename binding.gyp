{
  "targets": [
    {
      "target_name": "my_extension",
      "sources": ["my_extension.cc"],
      "conditions":[
        ["OS=='mac'", {
      	      "sources": [ "my_extension_mac.mm" ]
        }],
        ["OS=='win'", {
              "sources": ["my_extension_win.cc"],
      	  }]
      ], 
      "include_dirs" : ["<!(node -e \"require('nan')\")"]
    }
  ]
}
