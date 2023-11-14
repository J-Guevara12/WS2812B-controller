// Elemenetos del HTML
const temperatureData = document.getElementById('temperature-data');
const ledCircle = document.getElementById('led-circle');
const setTimeButton = document.getElementById('set-time');
const valueInput = document.getElementById('value-input');
const sendBrihtnessButton = document.getElementById('send-value-brightness');
var sendCredentialsButton = document.getElementById("send-credentials");
var ssdiInput =document.getElementById("ssdi-input");
var passwordInput = document.getElementById("value-password");

//Recibe e impirme los datos que se incresaron en los credenciales
sendCredentialsButton.addEventListener("click", function() {
    var ssdiValue = ssdiInput.value;
    var passwordValue = passwordInput.value;
    console.log("Value of the ssid; "+ ssdiValue);
    console.log("Value of Password: "+ passwordValue);

    const data = {
      "ssid": ssdiValue,
      "password": passwordValue,
    };

    const request = fetch("/api/connect",{
        method: "POST",headers:{ "Content-Type":"application/json"},
        body: JSON.stringify(data)
    })
})



//////////////////////////////////////////////////////////////////////////////////////////
//                     IMPLENTACION PARA CARGAR FIRMWARE  (OTA)

//seleccionar el archivo para cargar el firmware  por OTA
function getFileInfo() {
    var x = document.getElementById("selected_file");
    var file = x.files[0];
    document.getElementById("file_info").innerHTML = "<h4>File: " + file.name + "<br>" + "Size: " + file.size + " bytes</h4>";
}

// Manerjo de la  actulizacion de firmware

function updateFirmware() {
    var formData = new FormData();
    var fileSelect = document.getElementById("selected_file");
    
    if (fileSelect.files && fileSelect.files.length == 1) {
        var file = fileSelect.files[0];
        formData.set("file", file, file.name);
        document.getElementById("ota_update_status").innerHTML = "Uploading " + file.name + ", Firmware Update in Progress...";
        // Http Request
        var request = new XMLHttpRequest();
        request.upload.addEventListener("progress", updateProgress);
        request.open('POST', "/OTAupdate");
        request.responseType = "blob";
        request.send(formData);
    } else {
        window.alert('Select A File First')
    }
}

//Progreso de  tranferecnia al servidor del cliente
function updateProgress(oEvent) {
    if (oEvent.lengthComputable){
        getUpdateStatus();
    } else {
        window.alert('total size is unknown')
    }
}

//Estado de actulizacion de firmaware
function getUpdateStatus() {
    var xhr = new XMLHttpRequest();
    var requestURL = "/OTAstatus";
    xhr.open('POST', requestURL, false);
    xhr.send('ota_update_status');

    if (xhr.readyState == 4 && xhr.status == 200) {		
        var response = JSON.parse(xhr.responseText);				
	 	document.getElementById("latest_firmware").innerHTML = response.compile_date + " - " + response.compile_time
		// If flashing was complete it will return a 1, else -1
		// A return of 0 is just for information on the Latest Firmware request
        if (response.ota_update_status == 1){
    		// Set the countdown timer time
            seconds = 10;
            otaRebootTimer();
        } else if (response.ota_update_status == -1){
            document.getElementById("ota_update_status").innerHTML = "!!! Upload Error !!!";
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////

function showSelectedValue() {
    const selectElement = document.getElementById("miSelect");
    const selectedValue = selectElement.value;
    const resultElement = document.getElementById("result");
    resultElement.textContent = "Range " + selectedValue;
}



////////////////////////////////////////////////////////////////

let range1MinValue = 0;
let range1MaxValue = 30;

let range2MinValue = 31;
let range2MaxValue = 50;

let range3MinValue = 51;
let range3MaxValue = 70;

let R1_RGB = {
    "red": 255,
    "green": 0,
    "blue": 0,
}

let R2_RGB = {
    "red": 0,
    "green": 255,
    "blue": 0,
}

let R3_RGB = {
    "red": 0,
    "green": 0,
    "blue": 255,
}



///////////////////////////////////////////////////////////////////////////////////////////

function showSelectedValue() {
    const selectElement = document.getElementById("miSelect");
    const selectedValue = selectElement.value;
    const resultElement = document.getElementById("result");
    resultElement.textContent = "Range " + selectedValue;

    // Actualiza los valores de los sliders y  rangos  ingresados
    if (selectedValue === "temperature 1") {
        minRangeInput.value = range1MinValue;
        maxRangeInput.value = range1MaxValue;
        document.getElementById("redSlider").value = R1_RGB.red;
        document.getElementById("greenSlider").value = R1_RGB.green;
        document.getElementById("blueSlider").value = R1_RGB.blue;
    } else if (selectedValue === "temperature 2") {
        minRangeInput.value = range2MinValue;
        maxRangeInput.value = range2MaxValue;
        document.getElementById("redSlider").value = R2_RGB.red;
        document.getElementById("greenSlider").value = R2_RGB.green;
        document.getElementById("blueSlider").value = R2_RGB.blue;
    } else if (selectedValue === "temperature 3") {
        minRangeInput.value = range3MinValue;
        maxRangeInput.value = range3MaxValue;
        document.getElementById("redSlider").value = R3_RGB.red;
        document.getElementById("greenSlider").value = R3_RGB.green;
        document.getElementById("blueSlider").value = R3_RGB.blue;
    }
}

const sendButton = document.getElementById("value-range-temperature");

sendButton.addEventListener("click", () => {
    const selectedValue = document.getElementById("miSelect").value;
    const minValue = minRangeInput.value;
    const maxValue = maxRangeInput.value;

    // almacena  los valores  de cada rango
    if (selectedValue === "temperature 1") {
        range1MinValue = minValue;
        range1MaxValue = maxValue;
        R1_RGB.red = parseInt(document.getElementById("redSlider").value);
        R1_RGB.green = parseInt(document.getElementById("greenSlider").value);
        R1_RGB.blue = parseInt(document.getElementById("blueSlider").value);
    } else if (selectedValue === "temperature 2") {
        range2MinValue = minValue;
        range2MaxValue = maxValue;
        R2_RGB.red = parseInt(document.getElementById("redSlider").value);
        R2_RGB.green = parseInt(document.getElementById("greenSlider").value);
        R2_RGB.blue = parseInt(document.getElementById("blueSlider").value);
    } else if (selectedValue === "temperature 3") {
        range3MinValue = minValue;
        range3MaxValue = maxValue;
        R3_RGB.red = parseInt(document.getElementById("redSlider").value);
        R3_RGB.green = parseInt(document.getElementById("greenSlider").value);
        R3_RGB.blue = parseInt(document.getElementById("blueSlider").value);
    }

    minRangeValue.textContent = "Min: " + minValue;
    maxRangeValue.textContent = "Max: " + maxValue;


});



///////////////////////////////////////////////////////////////


// Función para actualizar temperatura y color del LED
function updateTemperatureAndLED(temperature) {
    temperatureData.textContent = temperature + ' °C';
    
    // Cambiar el color del LED en función de la temperatura
    if (temperature > range1MinValue && temperature < range1MaxValue) {
        sendRGBvalues(R1_RGB)
    } 
    else if (temperature > range2MinValue && temperature < range2MaxValue) {
        sendRGBvalues(R2_RGB)
    } 
    else if (temperature > range3MinValue && temperature < range3MaxValue){
        sendRGBvalues(R3_RGB)
    } 
}



async function getTemperature() {
  const response = await fetch("api/temperature");
  const res = await response.json();
  return res.temperature;
}



//  datos UART  mostrados
setInterval(async () => {
    const temperature = await getTemperature();
    updateTemperatureAndLED(temperature);
}, 100);  // Actualiza cada 5 segundos (



// datos del RGB

document.getElementById("redSlider").oninput = function() {
    const value = this.value;
    const selectedValue = document.getElementById("miSelect").value;
    if (selectedValue === "temperature 1") {
        R1_RGB.red = parseInt(value);
    } else if (selectedValue === "temperature 2") {
        R2_RGB.red = parseInt(value);
    } else if (selectedValue === "temperature 3") {
        R2_RGB.red = parseInt(value);
    }
    document.getElementById("red-slider-value").textContent = value;
};

document.getElementById("greenSlider").oninput = function() {
    const value = this.value;
    const selectedValue = document.getElementById("miSelect").value;
    if (selectedValue === "temperature 1") {
        R1_RGB.green = parseInt(value);
    } else if (selectedValue === "temperature 2") {
        R2_RGB.green = parseInt(value);
    } else if (selectedValue === "temperature 3") {
        R2_RGB.green = parseInt(value);
    }
    document.getElementById("green-slider-value").textContent = value;
};


document.getElementById("blueSlider").oninput = function() {
    const value = this.value;
    const selectedValue = document.getElementById("miSelect").value;
    if (selectedValue === "temperature 1") {
        R1_RGB.blue = parseInt(value);
    } else if (selectedValue === "temperature 2") {
        R2_RGB.blue = parseInt(value);
    } else if (selectedValue === "temperature 3") {
        R2_RGB.blue = parseInt(value);
    }
    document.getElementById("blue-slider-value").textContent = value;
};



const minRangeInput = document.getElementById("value-min-range");
const maxRangeInput= document.getElementById("value-max-range");
const minRangeValue = document.getElementById("min-range-value");
const maxRangeValue = document.getElementById("max-range-value");
const valueRange =document.getElementById("value-range");


sendButton.addEventListener("click",()=> {
    const minValue = minRangeInput.value;
    const maxValue = maxRangeInput.value;
    minRangeValue.textContent="Min: "+ minValue;
    maxRangeValue.textContent="Max: "+ maxValue;

});




// envio y solitud del  servidor
async function sendRGBvalues(data) {
    const response = await fetch("api/brightness",{
    method: "POST",headers:{ "Content-Type":"application/json"},
    body: JSON.stringify(data)});
    if (response.ok){
        return response.statusText
    }
}

  
