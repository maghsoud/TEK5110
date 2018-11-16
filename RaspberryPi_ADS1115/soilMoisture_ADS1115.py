import time
#import Adafruit library for ADS1115 analog to digital convertor
import Adafruit_ADS1x15
#Create an ADS1115 instance
adc=Adafruit_ADS1x15.ADS1115()
#

# Choose a gain of 1 for reading voltages from 0 to 4.09V.
# Or pick a different gain to change the range of voltages that are read:
#  - 2/3 = +/-6.144V
#  -   1 = +/-4.096V
#  -   2 = +/-2.048V
#  -   4 = +/-1.024V
#  -   8 = +/-0.512V
#  -  16 = +/-0.256V
# See table 3 in the ADS1015/ADS1115 datasheet for more info on gain.
#maximum digital output of ADC is 32767 
#Define ADC input line and the gain for Voltage sensor module
soilInput=0
soilGain=2/3  #when soil sensor connected to 5V

#Please enter voltage when soil sensor is 100% in water
soilSensor_maxVoltage=1.6
#Please enter voltage when soil sensor is dry
soilSensor_DryVoltage=4.8

#This section convert voltage read by  ADC to percentage (0-100%)
#for more information read Calculating Scale and Offset in  https://racelogic.support
#calculate voltage scale or gradient of line
#dy/dx=(100-0)/(soilSensor_maxVoltage-soilSensor_DryVoltage)
scale=100/(soilSensor_maxVoltage-soilSensor_DryVoltage)
#calculate ofsset: Y=mX+c -> percent=(scale*Voltage)+offset
offset=(-scale*soilSensor_DryVoltage)


while True:
    # Read ADC converted value from Analog Input for Voltage
    soil_adc=adc.read_adc(soilInput,soilGain)
    #Calculate actual voltage from ADC data
    soilMoistVoltage=((soil_adc*6.144)/32767)
    #Calculate soil moisture percentage
    soilMoist_percentage=(scale*soilMoistVoltage)+offset
    print('| {moisture} | {soil}| {soilPercent:.2f}%|  '.format(moisture=soil_adc,soil=soilMoistVoltage,soilPercent=soilMoist_percentage))
#    time.sleep(60)
    time.sleep(1)



