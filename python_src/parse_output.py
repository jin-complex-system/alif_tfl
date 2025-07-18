import os
import glob

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
        class_id, prediction = parse_file(filepath=filepath)

        y_test.append(class_id)
        y_pred.append(prediction)

    return y_pred, y_test

def create_confusion_matrix(
    y_pred,
    y_test,
    num_classes,
    output_directory,
    filename="confusion_matrix"
):
    """ Create confusin matrix figures from lists

    Arg:
        y_pred - 
        y_test - 
        num_classes - 
        output_directory - 
        filename - 
    """
    # Check parameters
    assert(len(y_pred) == len(y_test))

    # Import packages
    import tensorflow as tf
    import seaborn as sn
    import matplotlib.pyplot as plt

    # Constants
    CONFUSION_MATRIX_FIGURE_DPI=600
    TITLE="Confusion Matrix"
    FIGURE_FILENAME_WITH_ANNOTATION = "{}_annot.png".format(filename)
    FIGURE_FILENAME_NO_ANNOTATION = "{}_no_annot.png".format(filename)

    cm = tf.math.confusion_matrix(
        y_test,
        predictions=y_pred,
    )

    # Only use square if the size matches
    shape_of_confusion_matrix = tf.shape(cm)

    if (num_classes == shape_of_confusion_matrix[0] and num_classes == shape_of_confusion_matrix[1]):
        is_square = True
    else:
        is_square = False

    # Save with annotations
    plt.figure(dpi=CONFUSION_MATRIX_FIGURE_DPI)
    sn.heatmap(cm, annot=True, square=is_square)
    
    plt.xlabel('Prediction Label')
    plt.ylabel('True Label')
    plt.title(TITLE)
    # plt.show()
    plt.savefig(os.path.join(output_directory, FIGURE_FILENAME_WITH_ANNOTATION))
    plt.close()

    # Save with annotations
    plt.figure(dpi=CONFUSION_MATRIX_FIGURE_DPI)
    sn.heatmap(cm, annot=False, square=is_square)
    
    plt.xlabel('Prediction Label')
    plt.ylabel('True Label')
    plt.title(TITLE)
    # plt.show()
    plt.savefig(os.path.join(output_directory, FIGURE_FILENAME_NO_ANNOTATION))
    plt.close()


def _main(
        target_directory,
        output_directory,
        num_classes):

    # Make necessary directories
    os.makedirs(output_directory, exist_ok=True)

    y_pred, y_test = parse_files(target_directory=target_directory)

    create_confusion_matrix(
        y_pred=y_pred,
        y_test=y_test,
        num_classes=num_classes,
        output_directory=output_directory,
        filename="confusion_matrix",
    )
    print("Done")

if __name__ == '__main__':
    target_directory = "out_P"
    output_directory = "_my_results"
    num_classes = 19

    _main(
        target_directory=target_directory,
        output_directory=output_directory,
        num_classes=num_classes)