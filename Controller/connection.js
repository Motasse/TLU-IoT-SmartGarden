var client;
var onl_client;
var obj;
function connect(){
	client = new Paho.MQTT.Client("broker.hivemq.com", 8000, "cBase1");
	client.onConnectionLost = onConnectionLost;
	client.onMessageArrived = onMessageArrived;
	//client.onMessageDelivered = onMessageDelivered;
	var options = {
		onSuccess: onConnect,
		onFailure: onFail
	};
	client.connect(options);
}
function onConnect() {
	console.log("Connected");
	client.subscribe("flogro/json");
}
function onFail(){
	console.log("Vui lòng thử lại sau");
	connect();
}
function onConnectionLost(responseObject) {
	if (responseObject.errorCode !== 0) {
		console.log("onConnectionLost:"+responseObject.errorMessage);
		connect();
	}
}
function onMessageArrived(msg){
	console.log(msg.payloadString);
	obj = JSON.parse(msg.payloadString);
	update();
}
