var ip;
var port;
var ctrl = document.getElementsByName("ctrl");

function getAdd(){
	while (ip == null || ip == ""){
		ip = prompt ("Enter Broker IP");
	}
	while (port == null || port == ""){
		port = prompt("Enter Broker Port"); 
	}
}

function control(elem){
	if(elem.id === "pump"){
		if(elem.checked){
			var message = new Paho.MQTT.Message('{"pump":1}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("pumpstat").innerHTML = "Pump is on";
		}else{
			var message = new Paho.MQTT.Message('{"pump":0}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("pumpstat").innerHTML = "Pump is off";
		}
	}else if(elem.id === "upper"){
		if(elem.checked){
			var message = new Paho.MQTT.Message('{"upper":1}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("uppertray").innerHTML = "Uptrayout";
		}else{
			var message = new Paho.MQTT.Message('{"upper":0}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("uppertray").innerHTML = "Uptrayin";
		}
	}else if(elem.id==="lower"){
		if(elem.checked){
			var message = new Paho.MQTT.Message('{"lower":1}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("lowertray").innerHTML = "Lowtrayout";
		}else{
			var message = new Paho.MQTT.Message('{"lower":0}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("lowertray").innerHTML = "Lowtrayin";
		}
	}else if(elem.id==="v1"){
		if(elem.checked){
			var message = new Paho.MQTT.Message('{"valve":"v1on"}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("lower").innerHTML = "Lowtrayout";
		}else{
			var message = new Paho.MQTT.Message('{"valve":"v1off"}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("lower").innerHTML = "Lowtrayout";
		}
	}else if(elem.id==="v2"){
		if(elem.checked){
			var message = new Paho.MQTT.Message('{"valve":"v2on"}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("lower").innerHTML = "Lowtrayout";
		}else{
			var message = new Paho.MQTT.Message('{"valve":"v2off"}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("lower").innerHTML = "Lowtrayout";
		}
	}else if(elem.id==="v3"){
		if(elem.checked){
			var message = new Paho.MQTT.Message('{"valve":"v3on"}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("lower").innerHTML = "Lowtrayout";
		}else{
			var message = new Paho.MQTT.Message('{"valve":"v3off"}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("lower").innerHTML = "Lowtrayout";
		}
	}else if(elem.id==="auto"){
		if(elem.checked){
			var message = new Paho.MQTT.Message('{"auto":1}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("autostat").innerHTML = "Auto mode on";
			for(var i=0; i<ctrl.length; i++){
				ctrl[i].disabled=true;
			}
		}else{
			var message = new Paho.MQTT.Message('{"auto":0}');
			message.destinationName = "flogro/control";
			client.send(message);
			//document.getElementById("autostat").innerHTML = "Auto mode off";
			for(var i=0; i<ctrl.length; i++){
				ctrl[i].disabled=false;
			}
			getDat();
			
			
		}
	}
}
function getDat(){
	var message = new Paho.MQTT.Message('{"req_data":true}');
	message.destinationName = "flogro/control";
	client.send(message);
}
//setInterval(getDat, 10000);

function update(){
	document.getElementById("pump").checked = obj.pumpstate;
	document.getElementById("v1").checked = obj.valve[0];
	document.getElementById("v2").checked = obj.valve[1];
	document.getElementById("v3").checked = obj.valve[2];
	document.getElementById("upper").checked = obj.upper;
	document.getElementById("lower").checked = obj.lower;
	document.getElementById("auto").checked = obj.auto;
	document.getElementById("light").innerHTML = obj.sensor.lightRead+" lux";
	document.getElementById("rain").innerHTML = obj.sensor.rainRead+"%";
	for (var i=0; i<3; i++){
		document.getElementById("humid"+i).innerHTML = "Floor "+(i+1)+": "+obj.sensor.moist[i]+"%";
		//console.log(obj.sensor.moist[i]);
	}
}

function fontplus(){
	for (var i=0; i<document.getElementsByName('topic').length; i++){
		document.getElementsByName("topic")[i].style.fontSize = "larger";
	}
}
function fontminus(){
	for (var i=0; i<document.getElementsByName('topic').length; i++){
		document.getElementsByName("topic")[i].style.fontSize = "smaller";
	}
}

google.charts.load('current', {packages: ['corechart', 'line']});
google.charts.setOnLoadCallback(drawBasic);
function drawBasic() {
	var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
    var data = new google.visualization.DataTable();
    data.addColumn('number', 'X');
    data.addColumn('number', '%');
    data.addRows([
      [Date.now(), obj.sensor.moist[0]]
    ]);
    var options = {
      hAxis: {
        title: 'Time'
      },
      vAxis: {
        title: 'Humidity(%)'
      }
    };
    chart.draw(data, options);
}
