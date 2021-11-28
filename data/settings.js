// Supports settings page

toggleLoader("on");
var unloadingState = false;
var loaded = 0; // Hold data load status
var numReq = 2; // Number of JSON required
var hostname = window.location.hostname;
var originalHostnameConfig;
var hashLoc;
var posted = false;

// Tab tracking
var previousTab = "";
var currentTab = "";

// Attach the event after the page loads (multi-browser)
if (window.addEventListener) window.addEventListener("load", loadPage, false);
else if (window.attachEvent) window.attachEvent("onload", loadPage);
else window.onload = loadPage;

// Handle unloading page while making a getJSON call
$(window).bind("beforeunload", function () {
    unloadingState = true;
});

$('a[data-toggle="tab"]').on("shown.bs.tab", function (event) {
    previousTab = currentTab;
    currentTab = $(event.target).text();

    var url = $(event.target).attr("href"); // URL of activated tab
    var hashLoc = url.substring(url.indexOf("#")); // Get hash
    updateHelp(hashLoc); // Set context-sensitive help
});

// Turn off tooltips on radio button change {" "}
$("input[type=radio]").change(function () {
    //$('[data-toggle="tooltip"], .tooltip').tooltip("hide");
});

function fanClick(id) {
    if (id == "enablefancontrol0" || id == "enablefancontrol1") {
        if ($('input:radio[name="enablefancontrol"]')[0].checked) {
            document.getElementById("pidf").disabled = false;
            $("#fanrange").slider("enable");
        } else {
            document.getElementById("pidf").disabled = true;
            $("#fanrange").slider("disable");
        }
    } else if (id == "proportionalfan0" || id == "proportionalfan1") {
        if ($('input:radio[name="proportionalfan"]')[0].checked) {
            $("#fanrange").slider("enable");
        } else {
            $("#fanrange").slider("disable");
        }
    }
}

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
    // $(document).tooltip({
    //     'selector': '[data-toggle=tooltip]',
    //     'toggleEnabled': true
    // });
    populateConfig();
    populateTemps();
    loadHash();
    pollComplete();
}

function repopulatePage(doSpinner = false) {
    // Reload data if we need it
    if (doSpinner) {
        toggleLoader("on");
    }
    loaded = 0;
    populateConfig();
    populateTemps();
    pollComplete();
}

function loadHash() {
    // Link to tab via hash value
    var url = document.location.toString();
    if (url.match("#")) {
        $('.nav-tabs a[href="#' + url.split("#")[1] + '"]').tab("show");
    }

    // Change hash for page-reload
    $(".nav-tabs a").on("shown.bs.tab", function (e) {
        window.location.hash = e.target.hash;
    });
}

function populateConfig(callback = null) {
    // Get configuration settings
    var url = "/api/v1/config/";
    var config = $.getJSON(url, function () {
        configAlert.warning();
    })
        .done(function (config) {
            try {
                $('input[name="hostname"]').val(config.snugconfig.hostname);
                originalHostnameConfig = $('input[name="hostname"]').val();
                $('input[name="yardname"]').val(config.snugconfig.yardname);
                $('input[name="hivename"]').val(config.snugconfig.hivename);
                if (config.snugconfig.imperial) {
                    imperial = true;
                    localStorage.setItem("imperial", "true");
                    $('input:radio[name="imperial"]')[1].checked = true;
                } else {
                    imperial = false;
                    localStorage.setItem("imperial", "false");
                    $('input:radio[name="imperial"]')[0].checked = true;
                }

                if (imperial) {
                    $('input[name="setpoint"]').val(
                        parseFloat(config.temps.setpoint).toFixed(1)
                    );
                    $('input[name="treatpoint"]').val(
                        parseFloat(config.temps.treatpoint).toFixed(1)
                    );
                    $('input[name="outputmax"]').val(
                        parseFloat(config.temps.outputmax).toFixed(1)
                    );
                    $('input[name="calambient"]').val(
                        parseFloat(config.temps.ambientcal).toFixed(1)
                    );
                    $('input[name="calupper"]').val(
                        parseFloat(config.temps.uppercal).toFixed(1)
                    );
                    $('input[name="calcenter"]').val(
                        parseFloat(config.temps.centercal).toFixed(1)
                    );
                    $('input[name="callower"]').val(
                        parseFloat(config.temps.lowercal).toFixed(1)
                    );
                    $('input[name="caloutput"]').val(
                        parseFloat(config.temps.outputcal).toFixed(1)
                    );
                } else {
                    // Convert to C
                    $('input[name="setpoint"]').val(
                        convertFtoC(parseFloat(config.temps.setpoint)).toFixed(1)
                    );
                    $('input[name="treatpoint"]').val(
                        convertFtoC(parseFloat(config.temps.treatpoint)).toFixed(1)
                    );
                    $('input[name="outputmax"]').val(
                        convertFtoC(parseFloat(config.temps.outputmax)).toFixed(1)
                    );
                    $('input[name="calambient"]').val(
                        convertCalFtoC(parseFloat(config.temps.ambientcal)).toFixed(1)
                    );
                    $('input[name="calupper"]').val(
                        convertCalFtoC(parseFloat(config.temps.uppercal)).toFixed(1)
                    );
                    $('input[name="calcenter"]').val(
                        convertCalFtoC(parseFloat(config.temps.centercal)).toFixed(1)
                    );
                    $('input[name="callower"]').val(
                        convertCalFtoC(parseFloat(config.temps.lowercal)).toFixed(1)
                    );
                    $('input[name="caloutput"]').val(
                        convertCalFtoC(parseFloat(config.temps.outputcal)).toFixed(1)
                    );
                }

                $('input[name="treatperiod"]').val(parseInt(config.temps.treatperiod));
                $('input[name="coolperiod"]').val(parseInt(config.temps.coolperiod));

                if (config.temps.enablemaxout) {
                    $('input:radio[name="enablemaxout"]')[0].checked = true;
                } else {
                    $('input:radio[name="enablemaxout"]')[1].checked = true;
                }

                $('select[name="controlpoint"] option[value=' + parseInt(config.temps.controlpoint, 10) + "]").attr("selected", "selected");
                if (config.temps.controlenabled) {
                    $('input:radio[name="enablecontrol"]')[0].checked = true;
                } else {
                    $('input:radio[name="enablecontrol"]')[1].checked = true;
                }

                if (config.snugconfig.fancontrol) {
                    $('input:radio[name="enablefancontrol"]')[0].checked = true;
                } else {
                    $('input:radio[name="enablefancontrol"]')[1].checked = true;
                }

                if (config.snugconfig.propfan) {
                    $('input:radio[name="proportionalfan"]')[0].checked = true;
                } else {
                    $('input:radio[name="proportionalfan"]')[1].checked = true;
                }
                
                $("#fanrange").slider({});
                var range = [];
                range[0] = parseInt(config.snugconfig.fanrangelow);
                range[1] = parseInt(config.snugconfig.fanrangehigh)
                $("#fanrange").slider({ value: range });
                $('#fanrange').slider('refresh');

                // Handle enabling/disabling fan proportional and range control
                if ($('input:radio[name="enablefancontrol"]')[0].checked) {
                    $("#proportionalfan").prop( "disabled", false );
                    if ($('input:radio[name="proportionalfan"]')[0].checked = true) {
                        $("#fanrange").prop( "disabled", false );
                    } else {
                        $("#fanrange").prop( "disabled", true );
                    }
                } else {
                    $("#proportionalfan").prop( "disabled", true );
                }

                if (config.temps.ambientenabled) {
                    $('input:radio[name="enableambient"]')[0].checked = true;
                } else {
                    $('input:radio[name="enableambient"]')[1].checked = true;
                }

                if (config.temps.upperenabled) {
                    $('input:radio[name="enableupper"]')[0].checked = true;
                } else {
                    $('input:radio[name="enableupper"]')[1].checked = true;
                }

                if (config.temps.centerenabled) {
                    $('input:radio[name="enablecenter"]')[0].checked = true;
                } else {
                    $('input:radio[name="enablecenter"]')[1].checked = true;
                }

                if (config.temps.lowerenabled) {
                    $('input:radio[name="enablelower"]')[0].checked = true;
                } else {
                    $('input:radio[name="enablelower"]')[1].checked = true;
                }

                if (config.temps.outputenabled) {
                    $('input:radio[name="enableoutput"]')[0].checked = true;
                } else {
                    $('input:radio[name="enableoutput"]')[1].checked = true;
                }
            } catch {
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
            if (typeof callback == "function") {
                callback();
            }
        });
}

function populateTemps(callback = null) {
    // Get configuration settings
    var url = "/api/v1/info/sensors/";
    var config = $.getJSON(url, function () {
        tempsAlert.warning();
    })
        .done(function (temps) {
            try {
                if (loaded < numReq) {
                    loaded++;
                    $('input[name="pwmheat"]').val(parseFloat(temps.pwmheat).toFixed(1));
                    $('input[name="pwmfan"]').val(parseFloat(temps.pwmfan).toFixed(1));
                }
                if (typeof callback == "function") {
                    callback();
                }
            } catch {
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
        });
}

function isIP(hostname) {
    // Bool: is this an IP address
    if (
        /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(
            hostname
        )
    ) {
        return true;
    } else {
        return false;
    }
}

function isMDNS(hostname) {
    // Bool: Is this an mDNS host name
    if (hostname.endsWith(".local")) {
        return true;
    } else {
        return false;
    }
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
}

// PUT Handlers:

function processPost(obj) {
    posted = false;
    hashLoc = window.location.hash;
    var $form = $(obj);
    url = $form.attr("action");

    $("button[id='submitSettings']").prop("disabled", true);
    $("button[id='submitSettings']").html(
        '<span class="spinner-border spinner-border-sm" role="status" aria-hidden="true"></span> Updating'
    );

    // Switch here for hashLoc
    switch (hashLoc) {
        case "#controller":
            processControllerPost(url, obj);
            break;
        case "#accesspoint":
            processAPPost(url, obj);
            break;
        case "#tempcontrol":
            processTempControlPost(url, obj);
            break;
        case "#outputcontrol":
            processOutputControlPost(url, obj);
            break;
        case "#fancontrol":
            processFanControlPost(url, obj);
            break;
        case "#sensorcontrol":
            processSensorControlPost(url, obj);
            break;
        case "#pwmheat":
            processPWMHeatPost(url, obj);
            break;
        case "#pwmfan":
            processPWMFanPost(url, obj);
            break;
        default:
            // Unknown hash location passed
            break;
    }
}

function processControllerPost(url, obj) {
    // Handle controller posts
    var oldImperial = localStorage.getItem("imperial") == "true";

    // Get form data
    var $form = $(obj),
        hostname = $form.find("input[name='hostname']").val(),
        yardname = $form.find("input[name='yardname']").val(),
        hivename = $form.find("input[name='hivename']").val(),
        controlnum = $form.find("select[name='controlnum']").val(),
        imperial = $('input:radio[name="imperial"]')[1].checked;

    // Hold some data about what we changed
    var reloadpage = false;
    var hostnamechanged = false;
    var unitschanged = false;

    // Decide if we changed units
    if (imperial == oldImperial) {
        unitschanged = false;
    } else {
        unitschanged = true;
    }

    // Decide if we accessed via IP or non-mDNS name
    var confirmText = "";
    if (hostname != originalHostnameConfig) {
        hostnamechanged = true;
        if (isIP(window.location.hostname)) {
            confirmText =
                "You have connected with an IP address. Changing the hostname may have unintended consequences. Do you want to proceed?";
            reloadpage = false;
        } else if (!isMDNS(window.location.hostname)) {
            confirmText =
                "You are not accessing the application using an mDNS name. Changing the hostname may have unintended consequences. Do you want to proceed?";
            reloadpage = false;
        } else {
            reloadpage = true;
        }
    }
    if (confirmText && !confirm(confirmText)) {
        // Bail out on post
        repopulatePage(true);
        return;
    } else {
        // Process put
        if (hostnamechanged) {
            toggleLoader("on");
        }
        originalHostnameConfig = hostname; // Pick up changed host name
        data = {
            hostname: hostname,
            yardname: yardname,
            hivename: hivename,
            controlnum: controlnum,
            imperial: imperial,
        };
        if (hostnamechanged && reloadpage) {
            var protocol = window.location.protocol;
            var path = window.location.pathname;
            var newpage = protocol + "//" + hostname + ".local" + path + hashLoc;
            putData(url, data, newpage);
        } else if (unitschanged) {
            putData(url, data, false, true);
        } else {
            putData(url, data, false, false, function () {
                toggleLoader("off");
            });
        }
    }
}

function processAPPost(url, obj) {
    // Handle Access Point posts

    // Get form data
    // var $form = $(obj),
    //     hostname = $form.find("input[name='hostname']").val(),
    //     yardname = $form.find("input[name='yardname']").val(),
    //     hivename = $form.find("input[name='hivename']").val(),
    //     controlnum = $form.find("select[name='controlnum']").val(),
    //     imperial = $('input:radio[name="imperial"]')[1].checked;

    // Hold some data about what we changed
    // var reloadpage = false;
    // var hostnamechanged = false;
    // var unitschanged = false;

    // Decide if we changed units
    // if (imperial == oldImperial) {
    //     unitschanged = false;
    // } else {
    //     unitschanged = true;
    // }

    // Decide if we accessed via IP or non-mDNS name
    // var confirmText = "";
    // if (hostname != originalHostnameConfig) {
    //     hostnamechanged = true;
    //     if (isIP(window.location.hostname)) {
    //         confirmText =
    //             "You have connected with an IP address. Changing the hostname may have unintended consequences. Do you want to proceed?";
    //         reloadpage = false;
    //     } else if (!isMDNS(window.location.hostname)) {
    //         confirmText =
    //             "You are not accessing the application using an mDNS name. Changing the hostname may have unintended consequences. Do you want to proceed?";
    //         reloadpage = false;
    //     } else {
    //         reloadpage = true;
    //     }
    // }

    // if (confirmText && !confirm(confirmText)) {
    //     // Bail out on post
    //     repopulatePage(true);
    //     return;
    // } else {
    //     // Process put
    //     if (hostnamechanged) {
    //         toggleLoader("on");
    //     }
    //     originalHostnameConfig = hostname; // Pick up changed host name
    //     data = {
    //         hostname: hostname,
    //         yardname: yardname,
    //         hivename: hivename,
    //         controlnum: controlnum,
    //         imperial: imperial,
    //     };
    //     if (hostnamechanged && reloadpage) {
    //         var protocol = window.location.protocol;
    //         var path = window.location.pathname;
    //         var newpage = protocol + "//" + hostname + ".local" + path + hashLoc;
    //         putData(url, data, newpage);
    //     } else if (unitschanged) {
    //         putData(url, data, false, true);
    //     } else {
    //         putData(url, data, false, false, function () {
    //             toggleLoader("off");
    //         });
    //     }
    // }
    console.log("DEBUG: Fired the processAPPost() handler.")
}

function processTempControlPost(url, obj) {
    // Handle temperature control posts
    var imperial = localStorage.getItem("imperial") == "true";

    // Get form data
    if (imperial) {
        var $form = $(obj),
            setpoint = $form.find("input[name='setpoint']").val(),
            treatpoint = $form.find("input[name='treatpoint']").val(),
            treatperiod = $form.find("input[name='treatperiod']").val(),
            coolperiod = $form.find("input[name='coolperiod']").val(),
            controlpoint = $form.find("select[name='controlpoint']").val(),
            enablecontrol = $form.find("input[name='enablecontrol']:checked").val();
    } else {
        // Convert to F
        var $form = $(obj),
            setpoint = convertCtoF($form.find("input[name='setpoint']").val()),
            treatpoint = convertCtoF($form.find("input[name='treatpoint']").val()),
            treatperiod = $form.find("input[name='treatperiod']").val(),
            coolperiod = $form.find("input[name='coolperiod']").val(),
            controlpoint = $form.find("select[name='controlpoint']").val(),
            enablecontrol = $form.find("input[name='enablecontrol']:checked").val();
    }

    // Process put
    data = {
        setpoint: setpoint,
        treatpoint: treatpoint,
        treatperiod: treatperiod,
        coolperiod: coolperiod,
        controlpoint: controlpoint,
        enablecontrol: enablecontrol,
    };
    putData(url, data, false, false);
}

function processOutputControlPost(url, obj) {
    // Handle output control posts
    var imperial = localStorage.getItem("imperial") == "true";

    // Get form data
    if (imperial) {
        var $form = $(obj),
            outputmax = $form.find("input[name='outputmax']").val(),
            enablemaxout = $form.find("input[name='enablemaxout']:checked").val();
    } else {
        // Convert to F
        var $form = $(obj),
            outputmax = convertCtoF($form.find("input[name='outputmax']").val()),
            enablemaxout = $form.find("input[name='enablemaxout']:checked").val();
    }

    // Process put
    data = {
        outputmax: outputmax,
        enablemaxout: enablemaxout,
    };
    putData(url, data, false, false);
}

function processFanControlPost(url, obj) {
    // Handle fan control posts

    // Get form data
    var $form = $(obj),
        enablefancontrol = $form.find("input[name='enablefancontrol']:checked").val(),
        proportionalfan = $form.find("input[name='proportionalfan']:checked").val(),
        fanrange = $('#fanrange').attr("data-value").split(",")

    fanrangehigh = Math.max(...fanrange);
    fanrangelow = Math.min(...fanrange);

    // Process put
    data = {
        enablefancontrol: enablefancontrol,
        proportionalfan: proportionalfan,
        fanrangelow: fanrangelow,
        fanrangehigh: fanrangehigh
    };
    putData(url, data, false, false);
}

function processSensorControlPost(url, obj) {
    // Handle sensor control posts
    var imperial = localStorage.getItem("imperial") == "true";

    // Get form data
    if (imperial) {
        var $form = $(obj),
            calambient = $form.find("input[name='calambient']").val(),
            calupper = $form.find("input[name='calupper']").val(),
            calcenter = $form.find("input[name='calcenter']").val(),
            callower = $form.find("input[name='callower']").val(),
            caloutput = $form.find("input[name='caloutput']").val(),

            enableambient = $form.find("input[name='enableambient']:checked").val(),
            enableupper = $form.find("input[name='enableupper']:checked").val(),
            enablecenter = $form.find("input[name='enablecenter']:checked").val(),
            enablelower = $form.find("input[name='enablelower']:checked").val(),
            enableoutput = $form.find("input[name='enableoutput']:checked").val();
    } else {
        // Convert to F
        var $form = $(obj),
            calambient = convertCalCtoF($form.find("input[name='calambient']").val()),
            calupper = convertCalCtoF($form.find("input[name='calupper']").val()),
            calcenter = convertCalCtoF($form.find("input[name='calcenter']").val()),
            callower = convertCalCtoF($form.find("input[name='callower']").val()),
            caloutput = convertCalCtoF($form.find("input[name='caloutput']").val()),

            enableambient = $form.find("input[name='enableambient']:checked").val(),
            enableupper = $form.find("input[name='enableupper']:checked").val(),
            enablecenter = $form.find("input[name='enablecenter']:checked").val(),
            enablelower = $form.find("input[name='enablelower']:checked").val(),
            enableoutput = $form.find("input[name='enableoutput']:checked").val();
    }

    // Process put
    data = {
        calambient: calambient,
        enableambient: enableambient,
        calupper: calupper,
        enableupper: enableupper,
        calcenter: calcenter,
        enablecenter: enablecenter,
        callower: callower,
        caloutput: caloutput,
        enablelower: enablelower,
        enableoutput: enableoutput,
    };
    putData(url, data, false, false);
}

function processPWMHeatPost(url, obj) {
    // Handle pwmheat control posts

    // Get form data
    var $form = $(obj),
        pwmheat = $form.find("input[name='pwmheat']").val()
    // Process put
    data = {
        pwmheat: pwmheat,
    };
    putData(url, data, false, false);
    $('input:radio[name="enablecontrol"]')[1].checked = true;
}

function processPWMFanPost(url, obj) {
    // Handle pwmfan control posts

    // Get form data
    var $form = $(obj),
        pwmfan = $form.find("input[name='pwmfan']").val()
    // Process put
    data = {
        pwmfan: pwmfan,
    };
    putData(url, data, false, false);
    $('input:radio[name="enablecontrol"]')[1].checked = true; // Turn off temp control
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
        $("button[id='submitSettings']").prop("disabled", false);
        $("button[id='submitSettings']").html("Update");
        posted = false;
        document.activeElement.blur();
    } else {
        setTimeout(buttonClearDelay, 500); // try again in 300 milliseconds
    }
}
