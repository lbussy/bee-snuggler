// Supports temps page

toggleLoader("on");
var unloadingState = false;
var imperial;
var loaded = 0;
var numReq = 2;
var labels = [];
var temperatures = [];
var scaleTemps = [];
var setpoint = 0;
var tempChart;
var chartReloadTimer = 10000; // Reload every 10 seconds

// Attach the event after the page loads
if (window.addEventListener)
    window.addEventListener("load", loadPage, false);
else if (window.attachEvent)
    window.attachEvent("onload", loadPage);
else window.onload = loadPage;

function loadPage() { // Make sure the page is 100% loaded
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

// Detect unloading state during getJSON
$(window).bind("beforeunload", function () {
    unloadingState = true;
});

function populatePage() { // Get page data
    $(document).tooltip({ // Enable tooltips
        'selector': '[data-toggle=tooltip]',
        'placement': 'left',
        'toggleEnabled': true
    });
    populateTemps();
    populateConfig();
    pollComplete();
}

function populateConfig() { // Get configuration settings
    var url = "/api/v1/config/";
    var config = $.getJSON(url, function () {
        configAlert.warning();
    })
        .done(function (config) {
            try {
                // Header text
                var headerText = 'Temperature Sensors for ';
                headerText += config.snugconfig.hivename;
                headerText += ' in ';
                headerText += config.snugconfig.yardname;
                $('#templistName').html(headerText);
            }
            catch {
                if (!unloadingState) {
                    configAlert.warning("Unable to parse configuration data.");
                }
            }
        })
        .fail(function () {
            if (!unloadingState) {
                configAlert.warning("Unable to retrieve configuration data.");
            }
        })
        .always(function () {
            // Can post-process here
            if (loaded < numReq) {
                loaded++;
            }
        });
}

function populateTemps(callback = null) { // Get configuration settings
    var url = "/api/v1/info/sensors/";
    var okToClear = false;
    if (labels.length) { // Clear arrays if we are re-running
        okToClear = true;
    }
    var config = $.getJSON(url, function () {
        tempsAlert.warning();
    })
        .done(function (temps) {
            try {
                if (okToClear) {
                    labels = [];
                    temperatures = [];
                    scaleTemps = [];
                }

                if (temps.imperial) {
                    imperial = true;
                } else {
                    imperial = false;
                }

                var length = Object.keys(temps.sensors).length
                var i;
                var hadTemp = false;
                for (i = 0; i < length; i++) {
                    if (temps.sensors[i].enable) {
                        hadTemp = true
                        if (temps.imperial && parseFloat(temps.sensors[i].value) == -196.6) {
                            labels.push(toTitleCase(temps.sensors[i].name) + " temp invalid");
                            temperatures.push(0);
                        } else if (!temps.imperial && parseFloat(temps.sensors[i].value) == -127) {
                            labels.push(toTitleCase(temps.sensors[i].name) + " temp invalid");
                            temperatures.push(0);
                        } else {
                            labels.push(toTitleCase(temps.sensors[i].name));
                            temperatures.push(parseFloat(temps.sensors[i].value));
                        }
                    }
                }
                if (!hadTemp) {
                    labels.push("No Sensors Enabled");
                    temperatures.push(0);
                }

                $('#controlPoint').text(toTitleCase(temps.sensors[temps.controlpoint].name));

                setpoint = parseFloat(temps.setpoint).toFixed(1);
                setpointLabel = "Setpoint: " + setpoint;
                if (imperial) {
                    setpointLabel += " ℉";
                } else {
                    setpointLabel += " ℃";
                }

                scaleTemps = temperatures;
                scaleTemps.push(parseFloat(setpoint));

                if (typeof callback == "function") {
                    callback();
                }
            }
            catch {
                if (!unloadingState) {
                    tempsAlert.warning("Unable to parse temperature data.");
                }
            }
        })
        .fail(function () {
            if (!unloadingState) {
                tempsAlert.warning("Unable to retrieve temperature data.");
            }
        })
        .always(function () {
            // Can post-process here
            if (loaded < numReq) {
                loaded++;
            }
        });
}

function updateScales(chart) {
    var yScale = chart.scales['y-axis-0'];
    chart.options.scales = {
        yAxes: [{
            display: true,
            ticks: {
                fontColor: "rgba(255,255,255,.50)",
                min: Math.floor(Math.min.apply(this, scaleTemps) - 2),
                max: Math.ceil(Math.max.apply(this, scaleTemps) + 1),
                callback: function (value, index, values) {
                    return value + "°";
                }
            }
        }],
        xAxes: [{
            ticks: {
               fontColor: "rgba(255,255,255,.50)",
            }
         }]
    };
    chart.update();
    yScale = chart.scales['y-axis-0'];
}

function doChart() { // Draw chart.js chart
    if (tempChart) {
        tempChart.data.datasets.forEach((dataset) => {
            // Update data
            dataset.data = [];
            dataset.data = temperatures;
        });
        tempChart.options.annotation.annotations.forEach((annotations) => {
            // Update annotations
            annotations.label.content = setpointLabel;
            annotations.value = setpoint;
        });
        updateScales(tempChart);
        tempChart.update();
    } else {
        var ctx = document.getElementById("tempChart").getContext("2d");

        tempChart = new Chart(ctx, {
            type: 'bar',
            data: {
                labels: labels,
                datasets: [{
                    data: temperatures,
                    borderColor: 'rgba(221, 86, 0, 1)',
                    backgroundColor: 'rgba(221, 86, 0, 0.2)',
                    borderWidth: 2
                }]
            },
            options: {
                legend: false,
                responsive: true,

                title: {
                    display: false
                },

                onClick: barClick,

                tooltips: {
                    // yAlign: 'top',
                    displayColors: false,

                    callbacks: {
                        label: toolTip
                    }
                },

                scales: {
                    yAxes: [{
                        display: true,
                        ticks: {
                            fontColor: "rgba(255,255,255,.50)",
                            min: Math.floor(Math.min.apply(this, scaleTemps) - 2),
                            max: Math.ceil(Math.max.apply(this, scaleTemps) + 1),
                            callback: function (value, index, values) {
                                return value + "°";
                            }
                        }
                    }],
                    xAxes: [{
                        ticks: {
                           fontColor: "rgba(255,255,255,.50)",
                        }
                     }],
                },

                annotation: {
                    annotations: [{
                        type: 'line',
                        mode: 'horizontal',
                        scaleID: 'y-axis-0',
                        value: setpoint,
                        borderColor: '#4c9be8',
                        borderWidth: 4,
                        label: {
                            enabled: true,
                            content: setpointLabel
                        }
                    }]
                }
            }
        });
    }
}

function toolTip(tooltipItem, data) { // Callback for tool tips
    var index = tooltipItem.index;
    var label = [];

    var tempLine = '';
    if (temperatures[index]) {
        tempLine = (temperatures[index]).toFixed(1);
        if (imperial) {
            tempLine += " ℉";
        } else {
            tempLine += " ℃";
        }
        label.push(tempLine);
    }

    return label;
}

function barClick(event, array) { // Bar click handler
    var tapNum = array[0]._index;
    var url = "/settings/#sensorcontrol";
    window.open(url, "_self");
}

function clearState() {
    $("#coolstate").removeClass("alert-warning");
    $("#coolstate").removeClass("alert-danger");
    $("#coolstate").removeClass("alert-success");
    $("#coolstate").removeClass("alert-info");
    $("#coolstate").removeClass("alert-primary");
    $("#coolstate").removeClass("alert-secondary");
    $("#coolstate").removeClass("alert-light");
}

function pollComplete() {
    if (loaded == numReq) {
        finishPage();
    } else {
        setTimeout(pollComplete, 300); // try again in 300 milliseconds
    }
}

function chartReload() {
    populateTemps(function callFunction() {
        doChart();
        setTimeout(chartReload, chartReloadTimer);
    });
}

function finishPage() { // Display page
    toggleLoader("off");
    doChart();
    setTimeout(chartReload, chartReloadTimer);
}

function toTitleCase(str) {
    return str.replace(
        /\w\S*/g,
        function (txt) {
            return txt.charAt(0).toUpperCase() + txt.substr(1).toLowerCase();
        }
    );
}