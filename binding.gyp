{
  "targets": [
    {
      "target_name": "lnklib",
      "sources": [
        "src/lnklib.cc",
        "src/loader.cc"
      ],
      "conditions": [
        [ "OS == 'mac'", {
          "MACOSX_DEPLOYMENT_TARGET": "10.9",
          "xcode_settings": {
            "OTHER_CPLUSPLUSFLAGS" : [
              "-std=c++11"
            ]
          }
        }],
        [ "OS == 'linux'", {
          "cflags": [
            "-std=c++11"
          ]
        }]
      ]
    }
  ]
}