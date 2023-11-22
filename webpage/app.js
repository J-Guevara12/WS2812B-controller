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







///////////////////////////////////////////////////////////////


// Función para actualizar temperatura y color del LED
function updatecurrentsensor(current) {
    temperatureData.textContent = current + ' A';
    

async function getcurrent() {
  const response = await fetch("api/current");
  const res = await response.json();
  return res.current;
}



//  datos UART  mostrados
setInterval(async () => {
    const current = await getcurrent();
    updatecurrentsensor(current);
}, 100);  // Actualiza cada 5 segundos 




///////////////////////////////////////////////////////////////////////////////////////
// Estado del sistema
let systemStatus = false;

// Estado actual del consumo de energía
let currentEnergyMode = {
    "Mode 1": { "defCurrent": "", "defVoltage": "" },
    "Mode 2": { "defCurrent": "", "defVoltage": "" },
    "Mode 3": { "defCurrent": "", "defVoltage": "" }
};

// Estado actual de los sliders para cada opción de Pattern
let patternSliders = {
    "Pattern 1": { "red": 128, "green": 128, "blue": 128 },
    "Pattern 2": { "red": 128, "green": 128, "blue": 128 },
    "Pattern 3": { "red": 128, "green": 128, "blue": 128 },
    "Pattern 4": { "red": 128, "green": 128, "blue": 128 },
    "Pattern 5": { "red": 128, "green": 128, "blue": 128 },
    "Pattern 6": { "red": 128, "green": 128, "blue": 128 },
};

// Cambiar  estado del sistema (Encendido / Apagado)
function toggle() {
    systemStatus = !systemStatus;
    document.getElementById("status").innerText = systemStatus ? "Estado: Encendido" : "Estado: Apagado";
    document.getElementById("toggleBtn").innerText = systemStatus ? "Apagado" : "Encendido";
}

// Se camBian los valores de defCurrent y defVoltage según la opción seleccionada 
function changeEnergyMode() {
    const selectedMode = document.getElementById("selectModEnergy").value;
    currentEnergyMode[selectedMode].defCurrent = document.getElementById("defCurrent").value;
    currentEnergyMode[selectedMode].defVoltage = document.getElementById("defVoltage").value;
}

// Se  cambia los valores de los sliders según la opción seleccionada 
function changePatternSliders() {
    const selectedPattern = document.getElementById("selectPattern").value;
    document.getElementById("redSlider").value = patternSliders[selectedPattern].red;
    document.getElementById("greenSlider").value = patternSliders[selectedPattern].green;
    document.getElementById("blueSlider").value = patternSliders[selectedPattern].blue;
    updateSliderValues();
}

// Se actilizan los valores de los sliders
function updateSliderValues() {
    document.getElementById("red-slider-value").innerText = document.getElementById("redSlider").value;
    document.getElementById("green-slider-value").innerText = document.getElementById("greenSlider").value;
    document.getElementById("blue-slider-value").innerText = document.getElementById("blueSlider").value;
}

// Sen envian a la api los valores del modo de energia
async function sendEnergyModeJSON() {
    const selectedMode = document.getElementById("selectModEnergy").value;
    const jsonData = {
        defCurrent: currentEnergyMode[selectedMode].defCurrent,
        defVoltage: currentEnergyMode[selectedMode].defVoltage
    };

    const response = await fetch("api/energy-mode", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(jsonData)
    });

    if (response.ok) {
        return response.statusText;
    }
}

// Funcion para enviar el valor del patron de iluminado
async function sendPatternJSON() {
    const selectedPattern = document.getElementById("selectPattern").value;
    const jsonDataPattern = {
        red: patternSliders[selectedPattern].red,
        green: patternSliders[selectedPattern].green,
        blue: patternSliders[selectedPattern].blue
    };

    const response = await fetch("api/pattern", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(jsonDataPattern)
    });

    if (response.ok) {
        return response.statusText;
    }
}
document.getElementById("toggleBtn").addEventListener("click", toggle);
document.getElementById("selectModEnergy").addEventListener("change", changeEnergyMode);
document.getElementById("defCurrent").addEventListener("input", changeEnergyMode);
document.getElementById("defVoltage").addEventListener("input", changeEnergyMode);
document.getElementById("selectPattern").addEventListener("change", changePatternSliders);
document.getElementById("redSlider").addEventListener("input", updateSliderValues);
document.getElementById("greenSlider").addEventListener("input", updateSliderValues);
document.getElementById("blueSlider").addEventListener("input", updateSliderValues);
document.getElementById("send-credentials").addEventListener("click", sendEnergyModeJSON);
document.getElementById("selectPattern").addEventListener("click", sendPatternJSON);
