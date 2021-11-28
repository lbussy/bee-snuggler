// Supports mite treatment page

toggleLoader("on");
var unloadingState = false;
var reloadPeriod = 5 * 1000
var posted;
var loaded = 0; // Hold data load status
var numReq = 1; // Number of JSON required
var dotreatment = false;

// Attach the event after the page loads (multi-browser)
if (window.addEventListener) window.addEventListener("load", loadPage, false);
else if (window.attachEvent) window.attachEvent("onload", loadPage);
else window.onload = loadPage;

// Handle unloading page while making a getJSON call
$(window).bind("beforeunload", function () {
    unloadingState = true;
});

$(document).ready(function () {
    $('form').on('submit', function (e) {
        e.preventDefault();
    });
});

function loadPage() {
    // Page data load - make triple sure the whole DOM loaded
    if (document.readyState === "ready" || document.readyState === "complete") {
        populatePage();
    } else {
        document.onreadystatechange = function () {
            if (document.readyState == "complete") {
                populatePage();
            }
        };
    }
}

function populatePage() {
    // Get page data
    populateTreatment();
    pollComplete();
}

function repopulatePage(doSpinner = false) {
    // Reload data if we need it
    if (doSpinner) {
        toggleLoader("on");
    }
    loaded = 0;
    populateTreatment()
    pollComplete();
}

function populateTreatment(callback = null) {
    // Get treatment settings
    var url = "/api/v1/info/treatment/";
    var controlPointString = "";
    var pwmHeatString = "";
    var setPointTempString = "";
    var controlPointTempString = "";
    var outputTempString = "";
    var pwmFanString = "";
    var treatStateString = ""
    var config = $.getJSON(url, function () {
        tempsAlert.warning(); // Clear warning
    })
        .done(function (treatment) {
            try {
                if (loaded < numReq) {
                    dotreatment = treatment.dotreatment;
                    var submit = document.getElementById("submitSettings");
                    submit.classList.remove("btn-danger");
                    submit.classList.remove("btn-warning");
                    submit.classList.remove("btn-success");
                    $("button[id='submitSettings']").prop("disabled", false);
                    if (dotreatment) {
                        // Set button state
                        submit.classList.add("btn-danger");
                        $("button[id='submitSettings']").html("Cancel Treatment");
                    } else {
                        // Set button state
                        submit.classList.remove("btn-warning");
                        submit.classList.remove("btn-danger");
                        submit.classList.add("btn-success");
                        $("button[id='submitSettings']").html("Start Treatment");
                    }

                    // Temp Unit
                    if (treatment.imperial) {
                        tempUnit = "F";
                        disco = -196.6;
                    } else {
                        tempUnit = "C";
                        disco = -127;
                    }

                    // Treatment status

                    var treatPeriodString = "";
                    if (treatment.treatremaining == 0) {
                        treatPeriodString += "<1 of ";
                        treatPeriodString += parseInt(treatment.treatperiod);
                    } else {
                        treatPeriodString += "~"
                        treatPeriodString += treatment.treatremaining;
                        treatPeriodString += " of ";
                        treatPeriodString += parseInt(treatment.treatperiod);
                    }
                    treatPeriodString += " minutes remaining.";

                    var coolPeriodString = "";
                    if (treatment.coolremaining == 0) {
                        coolPeriodString += "<1 of ";
                        coolPeriodString += parseInt(treatment.coolperiod);
                    } else {
                        coolPeriodString += "~"
                        coolPeriodString += treatment.coolremaining;
                        coolPeriodString += " of ";
                        coolPeriodString += parseInt(treatment.coolperiod);
                    }
                    coolPeriodString += " minutes remaining.";

                    switch (parseInt(treatment.treatstate))
                    {
                        case 0:
                            treatStateString += "Idle";
                            break;
                        case 1:
                            treatStateString += "On: ";
                            treatStateString += treatPeriodString;
                            break;
                        case 2:
                            treatStateString += "Cooldown: ";
                            treatStateString += coolPeriodString;
                            break;
                        default:
                            treatStateString += "Unknown";
                            break;
                    }
                    
                    // Control point sensor
                    controlPointString += treatment.controlpoint.toProperCase();
                    controlPointString += " Hive Sensor:"

                    // Heat PWM
                    pwmHeatString += parseFloat(treatment.pwmheat.toFixed(0));
                    pwmHeatString += "%";

                    // Fan PWM
                    pwmFanString += parseFloat(treatment.pwmfan.toFixed(0));
                    pwmFanString += "%";

                    // Concatenate Setpoint and Controlpoint
                    if (! treatment.imperial) {
                        // Celsius values
                        setPointTempString = convertFtoC(parseFloat(treatment.setpoint.toFixed(1)));
                        controlPointTempString = convertFtoC(parseFloat(treatment.controlpointtemp.toFixed(1)));
                    } else {
                        // Fahrenheit values
                        setPointTempString = parseFloat(treatment.setpoint.toFixed(1));
                        controlPointTempString = parseFloat(treatment.controlpointtemp.toFixed(1));
                    }
                    setPointTempString +=  "\xB0 " + tempUnit;
                    controlPointTempString +=  "\xB0 " + tempUnit;

                    // Concatenate Outputtemp
                    if (treatment.outputtemp == disco) {
                        outputTempString = "--";
                    } else {
                        if (! treatment.imperial) {
                            outputTempString = convertFtoC(parseFloat(treatment.outputtemp.toFixed(1)));
                        } else {
                            outputTempString = parseFloat(treatment.outputtemp.toFixed(1));
                        }
                        outputTempString += "\xB0 " + tempUnit;
                    }
                }
                if (typeof callback == "function") {
                    callback();
                }
            } catch {
                if (!unloadingState) {
                    tempsAlert.warning("Unable to parse treatment data.");
                    treatStateString = "Error.";
                    controlPointString = "Error.";
                    pwmHeatString = "Error.";
                    pwmFanString = "Error.";
                    setPointTempString = "Error.";
                    controlPointTempString = "Error.";
                    outputTempString = "Error.";
                }
            }
        })
        .fail(function () {
            if (!unloadingState) {
                tempsAlert.warning("Unable to retrieve treatment data.");
                treatStateString = "Not loaded.";
                controlPointString = "Not loaded.";
                pwmHeatString = "Not loaded.";
                pwmFanString = "Not loaded.";
                setPointTempString = "Not loaded.";
                controlPointTempString = "Not loaded.";
                outputTempString = "Not loaded.";
            }
        })
        .always(function () {
            // Can post-process here
            $("#treatStateString").text(treatStateString);
            $("#controlPointString").text(controlPointString);
            $("#pwmHeatString").text(pwmHeatString);
            $("#pwmFanString").text(pwmFanString);
            $("#setPointTempString").text(setPointTempString);
            $("#controlPointTempString").text(controlPointTempString);
            $("#outputTempString").text(outputTempString);
            loaded++;
        });
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

// PUT Handlers:

function processPost(obj) {
    posted = false;
    var $form = $(obj);
    url = $form.attr("action");

    var submit = document.getElementById("submitSettings");
    $("button[id='submitSettings']").prop("disabled", true);
    submit.classList.remove("btn-danger");
    submit.classList.remove("btn-success");
    submit.classList.add("btn-warning");
    if (dotreatment) {
        $("button[id='submitSettings']").html('<span class="spinner-border spinner-border-sm" role="status" aria-hidden="true"></span> Stopping');
        $("#treatStateString").text("Stopping.");
        dotreatment = false
    } else {
        $("button[id='submitSettings']").html('<span class="spinner-border spinner-border-sm" role="status" aria-hidden="true"></span> Starting');
        $("#treatStateString").text("Starting.");
        dotreatment = true
    }

    // Process put
    data = {
        dotreatment: dotreatment,
    };
    putData(url, data, false, false);
}

function putData(url, data, newpage = false, newdata = false, callback = null) {
    var loadNew = newpage.length > 0;
    $.ajax({
        url: url,
        type: "PUT",
        data: data,
        success: function (data) {
            settingsAlert.error();
        },
        error: function (data) {
            settingsAlert.error("Settings update failed.");
        },
        complete: function (data) {
            if (loadNew) {
                window.location.href = newpage;
            } else if (newdata) {
                repopulatePage(true);
            }
            posted = true;
            buttonClearDelay();
            if (typeof callback == "function") {
                callback();
            }
        },
    });
}

function buttonClearDelay() {
    // Poll to see if entire page is loaded
    if (posted) {
        document.activeElement.blur();
        posted = false;
    } else {
        setTimeout(buttonClearDelay, 500); // try again in 300 milliseconds
    }
}
