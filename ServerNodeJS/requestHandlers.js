var querystring = require("querystring"),
    fs          = require("fs"),
	formidable  = require("formidable"),
	util		= require("util");
	
var sessionLogStream = fs.createWriteStream("sessionLogStream.txt", {flags: 'a', encoding: 'utf8', mode: 0666});

/* Client makes POST request to registerID on session start
 * Fields
 *   ID: <blank or previously supplied ID>
 *   Time: device time
 * Returns
 * 	 ID: a unique ID, echoes the supplied ID if supplied
 *
 */

function generateUUID() {
	// Generate a lexographically ascending uniqueID
	return (Date.now()) + 'x' + Math.round(Math.random() * 1E18);
}

function validateUUID(UUID) {
	// TODO: This should be more thorough. Check for 'x' ?
	return (UUID.length > 0);
}

function clientv1RegisterID(response, request) {
	console.log("Request handler 'clientv1RegisterID' was called")
		
	var form = new formidable.IncomingForm();
    form.parse(request, function(error, fields, files) {
		
		// Parse out sessionID supplied in registration request
		var sentUUID = fields['sessionID'];
		console.log("sentUUID: " + sentUUID); 

		// Generate a new sessionID for this session being registered
		var newUUID = generateUUID();
		console.log("newUUID: " + newUUID);
		
		// Log link betweensentUUID and newUUID
		if (validateUUID(sentUUID))
			sessionLogStream.write(sentUUID + " -> " + newUUID + "\n");
		else
			sessionLogStream.write("New Client: " + newUUID + "\n");
			
		// Respond back with new UUID
		response.writeHead(200, {'content-type': 'text/plain'});
		response.write(newUUID);
		response.end();
	});
}

/* Client makes POST request to periodically upload its data files
 * Fields
 * Returns
 *
 */

function clientv1UploadData(response, request) {
  console.log("Request handler 'clientv1UploadData' was called.");
  
  var form = new formidable.IncomingForm();
  console.log("about to parse");
  form.parse(request, function(error, fields, files) {
    console.log("parsing done");
    response.writeHead(200, {'content-type': 'text/plain'});
	var body;
	body  = 'received upload:\n\n';
	body += util.inspect({fields: fields, files: files});
	
	console.log(body);
    response.write(body);
    response.end();
  });
}

exports.clientv1RegisterID = clientv1RegisterID;
exports.clientv1UploadData = clientv1UploadData;