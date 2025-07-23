import os
import glob
from export_results import export_results

EDGE_FILE_EXT = 'edge'

def parse_file(
        filepath,
        signed=True):
    """ Parse file to create y_pred and y_test

    Arg:
        filepath - 
        signed - Individual bytes are signed or not

    return: 
        class_id - 
        prediction - 
    """
    assert(len(filepath) > 1)

    # Grab the class_id from filename
    basename = os.path.basename(filepath)
    class_id = int(basename.split('-')[1].strip())

    predicted_class = 0
    # Find the highest prediction from file
    with open(
        filepath,
        mode='rb',
        encoding=None) as fh:

        line = fh.read()
        best_prediction = -256

        for iterator in range(0, len(line)):
            byte_element = line[iterator]

            # Note that byte_element is rendered as unsigned integer by default
            # We need to change back to signed if relevant
            if signed:
                byte_element = int.from_bytes(bytes([line[iterator]]), byteorder='little', signed=True)
            
            if byte_element > best_prediction:
                best_prediction = byte_element
                predicted_class = iterator

    return class_id, predicted_class
        
def parse_files(
        target_directory,
        signed=True):
    """ Parse files to create y_pred and y_test

    Arg:
        filepath - 
        signed - Individual bytes are signed or not

    return: 
        y_pred - 
        y_test - 
    """
    assert(len(target_directory) > 1)

    y_pred = []
    y_test = []

    for filepath in glob.glob(os.path.join(target_directory, f'*.{EDGE_FILE_EXT}')):
        class_id, prediction = parse_file(
            filepath=filepath,
            signed=signed)

        y_test.append(class_id)
        y_pred.append(prediction)

    return y_pred, y_test

def _main(
        target_directory,
        output_directory):
    # Import libraries
    import pickle

    y_pred, y_test = parse_files(
        target_directory=target_directory,
        signed=True)
    assert (len(y_pred) == len(y_test))
    
    y_pred_filepath = os.path.join(output_directory, 'y_pred_file')
    y_test_filepath = os.path.join(output_directory, 'y_test_file')
    
    with open(y_pred_filepath, 'wb') as fp:
        pickle.dump(y_pred, fp)
    with open(y_test_filepath, 'wb') as fp:
        pickle.dump(y_test, fp)

if __name__ == '__main__':
    import os

    target_directory = "out_P"
    output_directory = os.path.join("_my_results", "SD_card_output")

    _main(
        target_directory=target_directory,
        output_directory=output_directory)
    
    print("Done")