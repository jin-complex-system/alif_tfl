import serial
import time

def parse_uart_for_results(
        target_comport,
        wait_timeout_seconds,
        max_length,
        debug=False):
    """ Parse UART for results

    Args:
        target_comport - chosen COMPORT
        wait_timeout_seconds - number of seconds to timeout before we end the communication; must be greater than 1
        max_length - exit out once length reaches max_length
        debug - If True, print debug statemenets
    """
    assert (len(target_comport) > 0)
    assert (wait_timeout_seconds > 1)
    assert (max_length > 0)

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
    
    while(num_seconds <= wait_timeout_seconds and len(y_test) <= max_length):
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
                continue
        else:
            num_seconds = 0
        
        # Parse for end of reading
        if "End of directory" in serial_string:
            print("Reached end of directory. Exiting")
            break

        # Parse for inference results
        class_id, predicted_result = parse_inference_results(serial_string)

        # Valid result
        if class_id > -1 and predicted_result > -1:
            y_test.append(class_id)
            y_pred.append(predicted_result)

            assert (len(y_test) == len(y_pred))
            print("Len: {}, Class id: {}, predicted_result: {}".format(len(y_test), class_id, predicted_result))

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

def _main(
        output_directory):
    
    # Import libraries
    # from export_results import export_results
    import pickle
    import os
    import time

    UART_WSL_COM_PORT = "COM9"
    os.makedirs(output_directory, exist_ok=True)

    start_time = time.time()
    y_pred, y_test = parse_uart_for_results(
        target_comport=UART_WSL_COM_PORT,
        wait_timeout_seconds=60,
        max_length=1600,
        debug=False)
    end_time = time.time()
    time_taken = end_time - start_time

    print("Time taken: {}".format(time_taken))
    
    y_pred_filepath = os.path.join(output_directory, 'y_pred_file')
    y_test_filepath = os.path.join(output_directory, 'y_test_file')

    print("Done")

if __name__ == '__main__':
    import os

    urbansound_directory = os.path.join("_my_results", "Urbansound_HE_Pre")
    _main(output_directory=urbansound_directory)

    # orbiwise_directory = os.path.join("_my_results", "Orbiwise_HE_Pre")
    # _main(output_directory=orbiwise_directory)

