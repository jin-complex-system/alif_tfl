import serial

def parse_uart_for_results(
        target_comport,
        wait_timeout_seconds,
        debug=False):
    """ Parse UART for results

    Args:
        target_comport - chosen COMPORT
        wait_timeout_seconds - number of seconds to timeout before we end the communication; must be greater than 1
        debug - If True, print debug statemenets
    """
    assert(len(target_comport) > 0)
    assert(wait_timeout_seconds > 1)

    # Constants
    READ_TIMEOUT_SECONDS = 2
    WRITE_TIMEOUT_SECONDS = 2
    
    print("Target port: {}".format(target_comport))

    serialPort = serial.Serial(
            port=target_comport,
            baudrate=115200,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=READ_TIMEOUT_SECONDS,
            # xonxoff=False,
            # rtscts=False,
            write_timeout=WRITE_TIMEOUT_SECONDS,
            # inter_byte_timeout=None,
        )

    # Reset serial port buffers
    serialPort.read_all()
    serialPort.reset_output_buffer()
    serialPort.reset_input_buffer()

    y_pred = []
    y_true = []

    num_seconds = 0
    while(num_seconds <= wait_timeout_seconds):
        # Read string
        serial_string = serialPort.readline()

        if debug:
            print(serial_string)
        serial_string = serial_string.decode("utf-8")
        serial_string = serial_string.strip('\n').strip('\r')

        if (serial_string is None or len(serial_string) == 0):
            num_seconds = num_seconds + READ_TIMEOUT_SECONDS

            if num_seconds > wait_timeout_seconds:
                print("Reached timeout")
                break
        else:
            num_seconds = 0
        
        # Parse for end of reading
        if "End of directory" in serial_string:
            print("Reached end of directory. Exiting")

        # Parse for inference results
        class_id, predicted_result = parse_inference_results(serial_string)

        if class_id > -1 and predicted_result > -1:
            if debug:
                print(serial_string)
                print("Class id: {}, predicted_result: {}".format(class_id, predicted_result))
            y_true.append(class_id)
            y_pred.append(predicted_result)
    serialPort.close()
    
    return y_pred, y_true

def parse_inference_results(
        target_string):
    """ Parse the string to extract inference results
    """

    class_id = -1
    predicted_result = -1

    if (len(target_string) < 0):
        return class_id, predicted_result
    
    string_elements = target_string.split(',')

    if len(string_elements) <= 2:
        return class_id, predicted_result

    class_id_string = string_elements[1].strip().split(':')[1]
    class_id = int(class_id_string)

    predicted_result_string = string_elements[2].strip().split(':')[1]
    predicted_result = int(predicted_result_string)

    return class_id, predicted_result

def _main():
    UART_WSL_COM_PORT = "/dev/ttyACM1"

    y_pred, y_true = parse_uart_for_results(
        target_comport=UART_WSL_COM_PORT,
        wait_timeout_seconds=300,
        debug=True)
    print("Done")

if __name__ == '__main__':
    _main()
