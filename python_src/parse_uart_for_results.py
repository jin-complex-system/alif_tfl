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
    y_test = []

    num_seconds = 0
    while(num_seconds <= wait_timeout_seconds):
        if (len(y_pred) > 10):
            print("End early")
            break

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
            y_test.append(class_id)
            y_pred.append(predicted_result)
    serialPort.close()
    
    return y_pred, y_test

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
    # Import libraries
    # from export_results import export_results
    import pickle
    import os

    UART_WSL_COM_PORT = "/dev/ttyACM1"
    output_directory = "_my_results"
    os.makedirs(output_directory, exist_ok=True)

    y_pred, y_test = parse_uart_for_results(
        target_comport=UART_WSL_COM_PORT,
        wait_timeout_seconds=300,
        debug=True)
    
    y_pred_filepath = os.path.join(output_directory, 'y_pred_file')
    y_test_filepath = os.path.join(output_directory, 'y_test_file')
    
    with open(y_pred_filepath, 'wb') as fp:
        pickle.dump(y_pred, fp)
    with open(y_test_filepath, 'wb') as fp:
        pickle.dump(y_test, fp)

    # Check that reading is as expected
    y_pred_read = list()
    y_test_read = list()

    with open(y_pred_filepath, 'rb') as fp:
        y_pred_read = pickle.load(fp)
    with open(y_test_filepath, 'rb') as fp:
        y_test_read = pickle.load(fp)

    assert (len(y_pred_read) == len(y_pred))
    assert (y_pred_read[1] == y_pred[1])

    assert (len(y_test_read) == len(y_test))
    assert (y_test_read[1] == y_test[1])

    print("Done")

if __name__ == '__main__':
    _main()
