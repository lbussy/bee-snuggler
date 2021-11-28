# The Victor
## *The Victor-For Varroa Mite Thermal Treatment NEW FEATURE Automatically shuts off after 3 hours*

[The Victor](http://www.greenbeehives.com/thevictor.html) is sold by [GreenBeehives.com](http://www.greenbeehives.com/) for $237.00 for the 8 or 10-frame variety.

The system is optimized for 12VDC operation in order that it may be powered by a 12-volt deep-cycle battery.  

---

## BOM

This BOM represents what would be required to reproduce the product.  Some reasonable replacements have been made.

- Controller: $2.23
- Relay Board: $1.80
- Heating Element: $4.97
- 4 x 120mm Fans: $29.28
- Jumper cables: $5.62

Total for major components: $43.90

In addition to these components, other common items such as wires, terminals, wood and hardware cloth are used.

## Temperature Controller

The brains of the box appear to be a China-sourced temperature controller with an NTC sensor and 20A 14DVC relay:

[W1219 W1209 DC 12V Dual LED Digital Thermostat Temperature Controller temperature Regulator Switch Control NTC 10K Sensor Module](https://www.aliexpress.com/item/32972182649.html) (multiple versions, choose dual-display) - $2.23 at the time this research was done.

[The same item was seen on Amazon Prime](https://smile.amazon.com/dp/B076Y5BXD9) for $11.99.

## Power Relay

Likely because the current load is close to the rated maximum for this board.  The relay on the board is a [Songle Power Relay SLA-05VDC-SL-A SLA-12VDC-SL-A SLA-24VDC-SL-A 24V 30A 240VAC 5PIN T90](https://www.aliexpress.com/item/1005001303743331.html), $0.44 at the time this research was done.

The relay is part of a high-current relay board assembly:  [30A 250V high current switch board normally closed relay board 12V input switch control 30A output](https://www.aliexpress.com/item/32836138463.html), $1.80 at the time this research was done.

[The same item was seen on Amazon Prime](https://smile.amazon.com/Yeeco-Contactor-Switching-electrical-switches/dp/B00MKWACVI/139-2217245-4741669) for $10.99.

## Heat Plate

The heat plate is a commodity-type flat resistance heater mounted upon an aluminum sheet.  These are sold for many purposes including pipe wraps and 3D printer heat beds.  The 12VDC application for these is not typical.  It is likely any DC heater could be applied with attention to the wattage.

Observed characteristics:

- 12DVC
- 0.8Ω
- 180W
- 15A

3D printer heat beds are often 24VDC, a 180W bed would have the following characteristics in a 24VDC version:

- 24DVC
- 2.3Ω
- 180W
- 7.5A

[Silicone Heater 3D Printer Heater](https://www.aliexpress.com/item/881899879.html), $38.54 at the time this research was done.

Other form factors are available which could provide similar results.  PTC heaters, specifically, may be a good choice.

Positive Temperature Coefficient (PTC) heaters are self-regulating heaters that run open-loop without any external diagnostic controls. While traditional fixed-resistance heaters employ wires and coils to generate heat, PTC heaters use conductive inks printed on thin, flexible polymer-based substrates. They are ideal for products that require safer, faster and more uniform heating. The properties of the material allow the PTC heater to act as its own sensor, eliminating the need for any external feedback controls. As a result, the heater inherently eliminates the risk of overheating.

Assuming the same heat density, a 120°C (248°F) aluminum PTC in 12VDC/180W capacity [77x62x6mm 12V 30W ~180W Thermostat Insulation Aluminum PTC Heater Element](https://www.aliexpress.com/item/32969999641.html) may be purchased for $4.97.

## Fans

120mm 12VDC fans are ubiquitous and no special brand or configuration is required.  The exact brand and model of the fans used in this device are the [uphere 3-Pack Long Life Computer Case Fan 120mm Cooling Case Fan for Computer Cases Cooling,12BK3-3](https://www.amazon.com/uphere-3-Pack-Computer-120mm-Cooling/dp/B072LDYKQ6) which is $12.99 for a three-pack on Amazon Prime.

*Four fans are used in this device.  The cost of $17.32 is calculated based on the price of four fans when purchasing at the three-pack price.

For this price point, these fans are a reasonable choice.  These are three-wire fans, which are intended to be used on computer motherboards which are able to sense the fan speed via the third wire.  This is not used nor necessary for this application.  The black or plain wire is ground, and the red or striped wire is the + DC voltage supply.

A replacement [12V Sleeve/Ball Cooling Fan 120x120x25mm 2PINDC Cooler Fan for PC Computer Case](https://www.aliexpress.com/item/4001262716733.html) may be purchased for $7.32.
