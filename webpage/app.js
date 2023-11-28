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
}


//  datos UART  mostrados
setInterval(async () => {
    const current = await getcurrent();
    updatecurrentsensor(current);
}, 100);  // Actualiza cada 5 segundos 



document.getElementById("MainR"),oninput=function(){
    const value = this.value;
    document.getElementById("red-slider-value-M").textContent= value;
    sendcolorPatternValues();
};

document.getElementById("MainG"),oninput=function(){
    const value = this.value;
    document.getElementById("green-slider-value-G").textContent= value;
    sendcolorPatternValues();
};

document.getElementById("MainB"),oninput=function(){
    const value = this.value;
    document.getElementById("blue-value-B").textContent= value;
    sendcolorPatternValues();
};

document.getElementById("SecondaryR"),oninput=function(){
    const value = this.value;
    document.getElementById("red-slider-value-S").textContent= value;
    sendcolorPatternValues();
};

document.getElementById("SecondaryG"),oninput=function(){
    const value = this.value;
    document.getElementById("green-slider-value-S").textContent= value;
    sendcolorPatternValues();
};

document.getElementById("SecondaryB"),oninput=function(){
    const value = this.value;
    document.getElementById("blue-slider-value-S").textContent= value;
    sendcolorPatternValues();
};


document.getElementById("BackgroundR"),oninput=function(){
    const value = this.value;
    document.getElementById("red-slider-value-B").textContent= value;
    sendcolorPatternValues();
};


document.getElementById("BackgroundG"),oninput=function(){
    const value = this.value;
    document.getElementById("green-slider-value-G").textContent= value;
    sendcolorPatternValues();
};

document.getElementById("BackgroundB"),oninput=function(){
    const value = this.value;
    document.getElementById("blue-slider-value-B").textContent= value;
    sendcolorPatternValues();
};


async function sendcolorPatternValues() {
    let index;
    let redValue, greenValue, blueValue;

    if (document.getElementById("MainR").checked || document.getElementById("MainG").checked || document.getElementById("MainB").checked) {
        index = 0; 
        redValue = parseInt(document.getElementById("MainR").value);
        greenValue = parseInt(document.getElementById("MainG").value);
        blueValue = parseInt(document.getElementById("MainB").value);
    } else if (document.getElementById("BackgroundR").checked || document.getElementById("BackgroundG").checked || document.getElementById("BackgroundB").checked) {
        index = 1; 
        redValue = parseInt(document.getElementById("BackgroundR").value);
        greenValue = parseInt(document.getElementById("BackgroundG").value);
        blueValue = parseInt(document.getElementById("BackgroundB").value);
    } else if (document.getElementById("SecondaryR").checked || document.getElementById("SecondaryG").checked || document.getElementById("SecondaryB").checked) {
        index = 2; // 
        redValue = parseInt(document.getElementById("SecondaryR").value);
        greenValue = parseInt(document.getElementById("SecondaryG").value);
        blueValue = parseInt(document.getElementById("SecondaryB").value);
    }

    const data = {
        id: index,
        R: redValue,
        G: greenValue,
        B: blueValue
    };
    console.log("Datos enviados por sendcolorPatternValues:", data);

    const response = await fetch("api/color", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(data)
    });
    
}

// Método GET  de los  valores de los colores de los patrones
async function getColorPatternValues() {
    const response = await fetch("api/color", {
        method: "GET",
        headers: { "Content-Type": "application/json" },
    });

    if (response.ok) {
        const data = await response.json();
        return data;
    } else {
        console.error("Error al obtener valores de color");
        return null;
    }
}
const colorValues = await getColorPatternValues();
console.log("Valores de los colores de los patrones:", colorValues);


document.getElementById("selectPattern").onchange = function() {
    const selectedValue = this.value;
    document.getElementById("selected-pattern-value").textContent = selectedValue;
    sendVarConfig(0,value);
};

document.getElementById("n-pulses-slider"),oninput=function(){
    const value = this.value;
    document.getElementById("number_of_pulses").textContent= value;
    sendVarConfig(2, value);
}


document.getElementById("pulses-l-sliders"),oninput=function(){
    const value = this.value;
    document.getElementById("pulse_length").textContent= value;
    sendVarConfig(1, value);
}

document.getElementById("period-slider"),oninput=function(){
    const value = this.value;
    document.getElementById("period_slider_value").textContent= value;
    sendVarConfig(3,value);
}


document.getElementById("selectselectcolored").onchange = function() {
    const selectedValue = this.value;
    document.getElementById("selected-colored-value").textContent = selectedValue;
    sendVarConfig(4,value);
};

async function sendVarConfig(key_v, value_v) {
    const data = {
      key: key_v,
      value: value_v,
    };
    console.log("Datos enviados por sendVarConfig:", data);
    const response = await fetch("api/config",{
    method: "POST",headers:{ "Content-Type":"application/json"},
    body: JSON.stringify(data)});
}

// Método GET para obtener los valores de configuracion.
async function getVarConfig() {
    const response = await fetch("api/config", {
        method: "GET",
        headers: { "Content-Type": "application/json" },
    });

    if (response.ok) {
        const data = await response.json();
        return data;
    } else {
        console.error("Error al obtener la configuración de variables");
        return null;
    }
}
const configValues = await getVarConfig();
console.log("Configuración de variables:", configValues);

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
