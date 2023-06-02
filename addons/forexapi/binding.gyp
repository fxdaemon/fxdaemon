{
  "targets": [
    {
      "target_name": "forexapi",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "conditions": [
        ["OS==\"linux\"", {
          "sources": ["src/common/winevent.cpp"],
          "libraries": ["-L<(module_root_dir)/lib/linux-x64", "-lForexConnect", "-lpricehistorymgr", "-lquotesmgr2"],
          "cflags": [ "-std=c++11", "-pthread", "-O2", "-Wall", "-Wl,--as-needed" ]
        }],
        ["OS==\"win\"", {
          "libraries": [
            "<(module_root_dir)/lib/win-x64/ForexConnect.lib",
            "<(module_root_dir)/lib/win-x64/pricehistorymgr.lib",
            "<(module_root_dir)/lib/win-x64/quotesmgr2.lib"
          ]
        }]
      ],
      "sources": [
        "src/common/EventQueue.cpp",
        "src/common/Mutex.cpp",
        "src/common/Utils.cpp",
        "src/table/Account.cpp",
        "src/table/Candle.cpp",
        "src/table/Offer.cpp",
        "src/table/Order.cpp",
        "src/table/Trade.cpp",
        "src/table/Summary.cpp",
        "src/table/Message.cpp",
        "src/CommunicatorListener.cpp",
        "src/CommunicatorStatusListener.cpp",
        "src/forexapi.cpp",
        "src/Order2Go.cpp",
        "src/PriceCollector.cpp",
        "src/ResponseListener.cpp",
        "src/SessionStatusListener.cpp",
        "src/TableListener.cpp",
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "<(module_root_dir)/include"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    },
    {
      "target_name": "copy",
      "type": "none",
      "dependencies" : [ "forexapi" ],
      "conditions": [
        ["OS==\"linux\"", {
          "copies": [
            {
              "destination": "<(module_root_dir)/dist/linux",
              "files": [ "<(module_root_dir)/build/Release/forexapi.node" ]
            }
          ]
        }],
        ["OS==\"win\"", {
          "copies": [
            {
              "destination": "<(module_root_dir)/dist/win32",
              "files": [ "<(module_root_dir)/build/Release/forexapi.node" ]
            }
          ]
        }]
      ]
    }
  ]
}