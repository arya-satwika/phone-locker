# Run these commands

## Prerequisites

- wokwi VS Code extension
- Arduino VS Code extension
- `arduino-cli` in terminal

## to initialize the project

``` bash
arduino-cli core update-index
arduino-cli core install arduino:avr
arduino-cli lib install "Servo" "LiquidCrystal I2C" "HX711"
```

## to compile

``` bash
arduino-cli compile --fqbn arduino:avr:mega -e --output-dir build/
```
