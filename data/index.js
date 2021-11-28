// Supports Index page
var reloadPeriod = 5 * 1000
var loaded = 0; // Hold data load status
var numReq = 1; // Number of JSON required
toggleLoader("on");

// Attach the event after the page loads
if (window.addEventListener)
    window.addEventListener("load", startLoad, false);
else if (window.attachEvent)
    window.attachEvent("onload", startLoad);
else window.onload = startLoad;

function startLoad() { // Make sure the page is 100% loaded
    if (document.readyState === 'ready' || document.readyState === 'complete') {
        populatePage();
    } else {
        document.onreadystatechange = function () {
            if (document.readyState == "complete") {
                populatePage();
            }
        }
    }
}

function populatePage() {
    populateIndex(); // Load data the first time
    pollComplete();
}

function repopulatePage(doSpinner = false) {
    // Reload data if we need it
    if (doSpinner) {
        toggleLoader("on");
    }
    loaded = 0;
    populateIndex()
    pollComplete();
}

function pollComplete() {
    // Poll to see if entire page is loaded
    if (loaded == numReq) {
        posted = true;
        finishPage();
    } else {
        setTimeout(pollComplete, 300); // try again in 300 milliseconds
    }
}

function finishPage() {
    // Display page
    toggleLoader("off");
    setTimeout(function(){ repopulatePage(false); }, reloadPeriod);
}

function populateIndex(callback = null) {
    var url = "/api/v1/info/sensors/";
    var data = $.getJSON(url, function () {
    })
        .done(function (data) {
            try {
                $("#indexTitle").text(data.title);

                var disco;
                var tempUnit;
                var setpoint;
                var treatpoint;
                var value = [];

                if (data.imperial) {
                    tempUnit = "F";
                    disco = -196.6;
                } else {
                    tempUnit = "C";
                    disco = -127;
                }

                // Perform display conversions
                if (!data.imperial) {
                    setpoint = convertFtoC(parseFloat(data.setpoint.toFixed(1)));
                    treatpoint = convertFtoC(parseFloat(data.treatpoint.toFixed(1)));
                    value[0] = convertFtoC(parseFloat(data.sensors[0].value.toFixed(1)));
                    value[1] = convertFtoC(parseFloat(data.sensors[1].value.toFixed(1)));
                    value[2] = convertFtoC(parseFloat(data.sensors[2].value.toFixed(1)));
                    value[3] = convertFtoC(parseFloat(data.sensors[3].value.toFixed(1)));
                    value[4] = convertFtoC(parseFloat(data.sensors[4].value.toFixed(1)));
                } else {
                    setpoint = parseFloat(data.setpoint.toFixed(1));
                    treatpoint = parseFloat(data.treatpoint.toFixed(1));
                    value[0] = parseFloat(data.sensors[0].value.toFixed(1));
                    value[1] = parseFloat(data.sensors[1].value.toFixed(1));
                    value[2] = parseFloat(data.sensors[2].value.toFixed(1));
                    value[3] = parseFloat(data.sensors[3].value.toFixed(1));
                    value[4] = parseFloat(data.sensors[4].value.toFixed(1));
                }

                // Setpoint display button
                if (data.controlenabled) {
                    if (setpoint == treatpoint) {
                        element.classList.add("btn-warning");
                        $("#setpoint").text("Treatment Setting: " + treatpoint + "\xB0 " + tempUnit);
                        $("#setpoint").addClass("noButton");
                        $("#setpoint").addClass("btn-warning");
                    } else {
                        $("#setpoint").text("Temperature Setting: " + setpoint + "\xB0 " + tempUnit);
                        $("#setpoint").addClass("noButton");
                        $("#setpoint").removeClass("btn-warning");
                    }
                }

                for (let i = 0; i < data.sensors.length; i++) {
                    var name = "";
                    if (i == data.controlpoint && data.controlenabled) {
                        var element = document.getElementById(data.sensors[i].name + "Row");
                        element.classList.remove("table-active");
                        element.classList.remove("table-default");
                        element.classList.add("table-warning");
                    }
                    name += data.sensors[i].name.toProperCase();
                    if (data.sensors[i].enable) {
                        if ((data.sensors[i].name != "ambient") && (data.sensors[i].name != "output")) {
                            name += " Hive"
                        }
                        name += ":"
                        if (value[i] == disco) {
                            $("#" + data.sensors[i].name).text("Not updated.");
                        } else {
                            $("#" + data.sensors[i].name).text(value[i] + "\xB0 " + tempUnit);
                        }
                    } else {
                        $("#" + data.sensors[i].name).text(" -- ");
                    }
                    $("#" + data.sensors[i].name + "Name").text(name);
                }
                $("#pwmheat").text(parseFloat(data.pwmheat).toFixed(0) + "%");
                $("#pwmfan").text(parseFloat(data.pwmfan).toFixed(0) + "%");
            }
            catch {
                $("#indexTitle").text("Error parsing data.");
                $("#ambient").text("Error parsing data.");
                $("#upper").text("Error parsing data.");
                $("#center").text("Error parsing data.");
                $("#lower").text("Error parsing data.");
                $("#output").text("Error parsing data.");
                $("#pwmheat").text("Error parsing data.");
                $("#pwmfan").text("Error parsing data.");
            }
        })
        .fail(function () {
            $("#indexTitle").text("Error loading data.");
            $("#ambient").text("Error loading data.");
            $("#upper").text("Error loading data.");
            $("#center").text("Error loading data.");
            $("#lower").text("Error loading data.");
            $("#output").text("Error loading data.");
            $("#pwmheat").text("Error loading data.");
            $("#pwmfan").text("Error loading data.");
        })
        .always(function () {
            // Post load processing here
            loaded++;
            toggleLoader("off");
        }
        );
}
