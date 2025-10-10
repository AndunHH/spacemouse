# Pyhton class to interface the space mouse with enabled ProgMode
import serial
import time

import logging

from enum import Enum

# the following two enums shall be equivalent to: spacemouse-keys/parameterMenu.h
class ProgmodeError(Enum):
    PE_OK = 10000
    PE_INVALID_PARAM = 10001
    PE_INVALID_VALUE = 10002
    PE_VALUE_FAULT = 10003
    PE_CMD_FAULT = 10004

class ParamType(Enum):
    BOOL = 1
    INT = 2
    FLOAT = 3

class SpaceMouseAPI:
    def __init__(self):
        """Initialize the SpaceMouseAPI instance."""
        self.ser = None
        self.magicNumber = None
        self.numberParameters = None
        self.actParamNo = None 

    def connect(self, port: str = None, baudrate: int = 115200, timeout: float = 1.0):
        """
        Connect to the SpaceMouse device.
        
        If a port is specified, use it directly.
        If not, call the `search()` function (to be implemented later).
        """
        if port is None:
            port = self.search()  # Placeholder for your own implementation
        
        try:
            self.ser = serial.Serial(port, baudrate=baudrate, timeout=timeout)
            time.sleep(0.1)  # Give the serial interface a short time to initialize
            logging.info(f"Connected to SpaceMouse on port {port}")
            self.quitAndFlush()
        except serial.SerialException as e:
            logging.warning(f"Failed to connect to SpaceMouse on port {port}: {e}")
            self.ser = None

    def close(self):
        """Close the serial connection if open."""
        if self.ser and self.ser.is_open:
            self.ser.close()
            logging.info("Serial connection closed.")
        else:
            logging.info("No open serial connection to close.")

    def quitAndFlush(self):
        # send three q's to leave all menus
        for i in range(0,3):
            self.send_and_receive("q")
            time.sleep(0.1)
        time.sleep(0.5)
        self.ser.reset_input_buffer()

    def send_and_receive(self, message: str) -> str:
        """
        Send a string to the device and return the received response line.
        
        Args:
            message (str): The string to send (newline added automatically if missing).
        
        Returns:
            str: The response received from the device, stripped of newline characters.
        """
        if not self.ser or not self.ser.is_open:
            raise ConnectionError("Serial connection not established.")

        # Ensure the message ends with newline
        if not message.endswith("\r\n"):
            message += "\r\n"

        logging.debug('Sending {0}'.format(message))

        # Send the message
        self.ser.write(message.encode("utf-8"))

        # Read a response line
        response = self.ser.readline().decode("utf-8", errors="ignore").strip()
        logging.debug('Received: {0}'.format(response))
        return response

    def search(self):
        """
        Placeholder search method. 
        To be implemented later by the user.
        Should return a serial port string.
        """
        #ports = [p.device for p in list(serial.tools.list_ports.comports())]
        #logging.info("Ports found: %s",ports)
        #for port in [(i, self.comboBox.itemText(i)) for i in range(self.comboBox.count())]:
        #mouse_settings = get_mouse_settings('/dev/ttyACM0')
        #if self.mouse_settings != 0:
        #    self.comboBox.setCurrentIndex(port[0])
        #    break
        raise NotImplementedError("search() method not implemented yet.")
    
    def sendCommand(self, cmd, value=None):
        if value is None:
            valueStr = ""
        elif isinstance(value, (int, float)):
            valueStr = str(value)
        elif isinstance(value, str):
            valueStr = value
        else:
            raise TypeError(f"Unsupported type for value: {type(value)}")
        
        received = self.send_and_receive('>'+cmd+valueStr)
        receivedCmd = received[1:2]
        if len(received) >2:
            payload = received[2:len(received)]
        else:
            payload = None
        
        #logging.debug('cmd: {0}'.format(cmd))
        #logging.debug('Received data raw: {0}'.format(received))
        #logging.debug('Received cmd: {0}'.format(receivedCmd))
        #logging.debug('Received length: {0}'.format(len(received)))
        #logging.debug('Received data: {0}'.format(returnValue))
        if receivedCmd == cmd:
            logging.debug('{0} acknowledged'.format(cmd))
            return self.interpret_serial_value(payload)
        else:
            logging.error('{0} not acknowledged'.format(cmd))
            time.sleep(1)
            logging.debug(self.ser.read_all())
            return None

        

    def parse_progmode_error(self, value: int):
        """
        Compare an incoming numeric value with the ProgmodeError enum.
        Return the matching enum object if found, otherwise None.
        Includes a debug print for visibility.
        """
        try:
            error_enum = ProgmodeError(value)
            logging.debug(f"Matched value {value} → {error_enum.name}")
            return error_enum
        except ValueError:
            logging.warning(f"Unknown ProgmodeError value: {value}")
            return None
        
    def interpret_serial_value(self, value_str: str):
        """
        Interpret a string received over the serial interface as:
        - Boolean (0 or 1)
        - Integer (-9999 ... +9999)
        - Float (xx.yyy)
        - ProgmodeError (values >= 10000)
        Returns the interpreted Python type (bool, int, or float),
        or None if parsing fails.
        Includes a debug print.
        """
        value_str = value_str.strip()

        # Try boolean (strict 0 or 1)
        if value_str in ("0", "1"):
            result = bool(int(value_str))
            logging.debug(f"Interpreted '{value_str}' as BOOL → {result}")
            return result
    
        # Try integer parsing (could be INT or Error Code)
        try:
            int_val = int(value_str)

            # Check for ProgmodeError
            if int_val >= 10000:
                try:
                    error_enum = ProgmodeError(int_val)
                    logging.debug(f"Interpreted '{value_str}' as PROGMODE_ERROR → {error_enum.name}")
                    return error_enum
                except ValueError:
                    # this might be given, when e.g. the magic number is queried
                    logging.debug(f"Unknown ProgmodeError value: {int_val}")
                    return int_val  # Return as plain int if not in enum

            # Normal integer range check
            if -9999 <= int_val <= 9999:
                logging.debug(f"Interpreted '{value_str}' as INT → {int_val}")
                return int_val

        except ValueError:
            pass

        # Try float (xx.yyy format)
        try:
            float_val = float(value_str)
            # Ensure it has a decimal point in the original string
            if "." in value_str:
                logging.debug(f"Interpreted '{value_str}' as FLOAT → {float_val}")
                return float_val
        except ValueError:
            pass

        # if everything else was not plausible, than it is a string
        logging.debug(f"Interpreting as a string:'{value_str}'")
        return value_str

    
    def getMagicNumber(self):
        ret = self.sendCommand(cmd='m')
        if isinstance(ret, int) and not isinstance(ret, bool):
            self.magicNumber = ret
            return self.magicNumber
        else:
            logging.error(f"Magic number not found. Error:{0}".format(ret))
            return None

    def getNumberParameters(self):
        ret = self.sendCommand(cmd='n')
        if isinstance(ret, int) and not isinstance(ret, bool):
            self.numberParameters = ret
            if self.numberParameters == 0:
                logging.error("Number of Parameters is zero! The EEPROM is probably cleared!")
            return self.numberParameters
        else:
            logging.error(f"Number of parameters not found. Error:{0}".format(ret))
            return None     

    def setParameterNumber(self, paramNo):
        if paramNo is None:
            logging.error('No Param No. provided')
            return False
        if self.numberParameters is None:
            self.getNumberParameters()

        if paramNo<1 or paramNo>self.numberParameters:
            #number 0 is not used!
            logging.error(f"Parameter Number {0} not in range 0 to {1}".format(paramNo, self.numberParameters))

        ret = self.sendCommand(cmd='p', value=paramNo)
        if ret == ProgmodeError.PE_OK:
            logging.debug(f"Set Parameter Number to {paramNo}")
            self.actParamNo = paramNo
            return True
        elif ret == ProgmodeError.PE_INVALID_PARAM:
            logging.warning(f"Unkown parameter number {0}".format(paramNo))
            self.actParamNo = None
            return False
        else:
            logging.warning(f"Unkown return type: {0}".format(ret))
            self.actParamNo = None
            return False
        
    def getParameterType(self, paramNo=None):
        # use new paramNo if provided
        if paramNo is not None:
            self.setParameterNumber(paramNo)

        # check if a new or stored paramNo is present
        if self.actParamNo is None:
            logging.error('No Param No. selected')
            return False
        
        ret = self.sendCommand(cmd='t')
        #logging.debug("ret is of type{0}".format(type(ret)))
        try:
            paramType = ParamType(ret)
            logging.debug(f"Interpreted '{ret}' as ParamType → {paramType.name}")
            return paramType
        except ValueError:
            
            logging.debug(f"Unknown ParamType value: {ret}")
            return ret  # Return as plain int if not in enum

    def getParameterDescription(self, paramNo=None):
        # use new paramNo if provided
        if paramNo is not None:
            self.setParameterNumber(paramNo)

        # check if a new or stored paramNo is present
        if self.actParamNo is None:
            logging.error('No Param No. selected')
            return False
        
        ret = self.sendCommand(cmd='d')
        if isinstance(ret,str):
            #yes, it is the description
            logging.debug(f"Description: '{ret}'")
            return ret  # Return as string
        else:
            logging.warning(f"Unknown return description: {ret}")
            return None
        

    def readValue(self, paramNo=None):
        # use new paramNo if provided
        if paramNo is not None:
            self.setParameterNumber(paramNo)

        # check if a new or stored paramNo is present
        if self.actParamNo is None:
            logging.error('No Param No. selected')
            return False
        
        #type = self.getParameterType_()
        ret = self.sendCommand(cmd='r')        
        return ret
    
    def writeValue(self, value, paramNo=None):
        # use new paramNo if provided
        if paramNo is not None:
            self.setParameterNumber(paramNo)
        else:
            self.setParameterNumber(self.actParamNo)

        # check if a new or stored paramNo is present
        if self.actParamNo is None:
            logging.error('No Param No. selected')
            return False
        
        paramType = self.getParameterType()
        logging.debug("write value {0} to paramNo {1}".format(value, paramNo))

        if (paramType == ParamType.BOOL) and isinstance(value, bool):
            value_str = 1 if value==True else 0 
            logging.debug("Write Value {0}".format(value_str))
        elif (paramType == ParamType.BOOL) and isinstance(value, (int,float)):
            value_str = 1 if value==1 else 0 
            logging.debug("Write Value {0}".format(value_str))
        elif (paramType == ParamType.INT) and isinstance(value, (int, float)):
            value_str = "{0:d}".format(value)
            logging.debug("Write Value {0}".format(value_str))
        elif (paramType == ParamType.FLOAT) and isinstance(value, (int, float)):
            value_str = "{0:.4f}".format(value)
            logging.debug("Write Value {0}".format(value_str))
        else:
            logging.error("value is of type{0}, but param is type {1}".format(paramType(value), paramType))
            return False
        return self.sendCommand(cmd='w',value=value_str)  
            
    def printAllParams(self):
        for i in range(1,sm.getNumberParameters()):
            print(f"#{i}: {sm.getParameterDescription(i)} <{sm.getParameterType(i).name}> = {sm.readValue(i)} ")

    def printParam(self, paramNo):
        print(f"#{paramNo}: {sm.getParameterDescription(paramNo)} <{sm.getParameterType(paramNo).name}> = {sm.readValue(paramNo)} ")

    def loadParamsFromEEPROM(self):
        ret = self.sendCommand(cmd='l')
        if ret == ProgmodeError.PE_OK:
            logging.debug("Loaded Params from EEPROM")
            return True
        else:
            logging.warning(f"Params not loaded:{0}".format(ret))
            return False
        
    def saveParamsToEEPROM(self):
        ret = self.sendCommand(cmd='s')
        if ret == ProgmodeError.PE_OK:
            logging.debug("Saved Params to EEPROM")
            return True
        else:
            logging.warning(f"Params not saved:{0}".format(ret))
            return False
        
    def invalidateMagicNumber(self):
        ret = self.sendCommand(cmd='i')
        if ret == ProgmodeError.PE_OK:
            logging.debug("Invalidated Magic Number")
            return True
        else:
            logging.warning(f"magic number not invalidated:{0}".format(ret))
            return False
        
    def clearEEPROM(self):
        ret = self.sendCommand(cmd='c')
        if ret == ProgmodeError.PE_OK:
            logging.debug("Cleared EEPROM")
            return True
        else:
            logging.warning(f"EEPROM not cleared{0}".format(ret))
            return False

if __name__ == '__main__':
    # change the debug logging here from logging.CRITICAL to logging.DEBUG
    logging.basicConfig(level=logging.CRITICAL, format='[%(asctime)s] [%(levelname)s] [%(funcName)s] %(message)s')
    
    sm = SpaceMouseAPI()
    sm.connect('/dev/ttyACM0')  # or sm.connect() if you implement search()

    # this commands can be enabled and tested one after the other
    #response = sm.send_and_receive(">m")
    #print(response)
    #sm.sendCommand(cmd='m')
    #sm.sendCommand(cmd='z')
    #sm.parse_progmode_error(10003)
    # sm.interpret_serial_value("0")
    # sm.interpret_serial_value("1")
    # sm.interpret_serial_value("-5465")
    # sm.interpret_serial_value("+555")
    # sm.interpret_serial_value("5+fdsf")
    # sm.interpret_serial_value("58.9878795")
    # sm.interpret_serial_value("10004")
    #print(f"Magic Number {sm.getMagicNumber()}")
    #print(f"Number of Parameters {sm.getNumberParameters()}")
    #if sm.setParameterNumber(5):
    #    print(f"Set Parameter Number to 5")

    #print(f"{sm.getParameterType()}")
    #print(f"{sm.getParameterDescription()}")
    #sm.setParameterNumber(55)
    #print(f"{sm.readValue(5)}")

    #Print all values
    sm.loadParamsFromEEPROM()
    #sm.printAllParams()
    #sm.printParam(32)
    #sm.printParam(2)
    #sm.printParam(3)
    #sm.printParam(18)
   
    #sm.writeValue(paramNo=32, value=201)
    #sm.writeValue(paramNo=32, value=-150)
    #sm.writeValue(paramNo=2, value=0.2)
    #sm.writeValue(paramNo=3, value=-1.23)
    #sm.writeValue(paramNo=18, value=True)

    #sm.printParam(32)
    #sm.printParam(2)
    #sm.printParam(3)
    #sm.printParam(18)
    sm.printAllParams()
    #sm.saveParamsToEEPROM()

    #sm.clearEEPROM()

    sm.close()