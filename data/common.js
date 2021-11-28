// Common Javascript elements

function convertCtoF(celsius) {
  var cTemp = celsius;
  var cToFahr = (cTemp * 9) / 5 + 32;
  return cToFahr;
}

function convertFtoC(fahrenheit) {
  var fTemp = fahrenheit;
  var fToCel = ((fTemp - 32) * 5) / 9;
  return fToCel;
}

function convertCalFtoC(fahrenheit) {
  // Zero-based conversion for the purposes of adding
  // or subtracting a calibration amount.
  // T(°C) = (T(°F) X (5/9)) - (32/9))
  return fahrenheit * 0.555;
}

function convertCalCtoF(celsius) {
  // Zero-based conversion for the purposes of adding
  // or subtracting a calibration amount.
  // T(°F) = T(°C) × 9/5 + 32
  return celsius * 1.8;
}

String.prototype.toProperCase = function(opt_lowerCaseTheRest) {
  return (opt_lowerCaseTheRest ? this.toLowerCase() : this)
    .replace(/(^|[\s\xA0])[^\s\xA0]/g, function(s){ return s.toUpperCase(); });
};
