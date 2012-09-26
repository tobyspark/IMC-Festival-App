var server = require("./server");
var router = require("./router");
var requestHandlers = require("./requestHandlers");

var handle = {}
handle["/registerID"]    = requestHandlers.clientv1RegisterID;
handle["/uploadData"]    = requestHandlers.clientv1UploadData;

server.start(router.route, handle);