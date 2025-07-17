import os
import glob

TFRECORD_EXT = 'tfrecord'

def parse_file(filepath):
    # Grab the class_id from filename
    basename = os.path.basename(filepath)
    class_id = int(basename.split('-')[1].strip())

    prediction = -256
    # Find the highest prediction from file
    with open(
        filepath,
        mode='rb',
        encoding=None) as fh:
        for line in fh:

            best_prediction = -256
            for byte_element in line:
                if (int(byte_element) > best_prediction):
                    prediction = int(byte_element)
    return class_id, prediction
        
def parse_files(target_directory):
    y_pred = []
    y_test = []

    for filepath in glob.glob(os.path.join(target_directory, f'*.{TFRECORD_EXT}')):
        class_id, prediction = parse_file(filepath=filepath)

        y_test.append(class_id)
        y_pred.append(prediction)

    return y_pred, y_test

def _main(
        target_directory,
        output_directory,
        num_classes):
    y_pred, y_test = parse_files(target_directory=target_directory)

    # Import packages
    import tensorflow as tf
    import seaborn as sn
    import matplotlib.pyplot as plt

    # Constants
    CONFUSION_MATRIX_FIGURE_DPI=600
    TITLE="Confusion Matrix"
    FIGURE_FILENAME_WITH_ANNOTATION = "confusion_matrix_annot.png"
    FIGURE_FILENAME_NO_ANNOTATION = "confusion_matrix_no_annot.png"

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

if __name__ == '__main__':
    target_directory = "out_P"
    output_directory = "_my_results"
    num_classes = 19

    _main(
        target_directory=target_directory,
        output_directory=output_directory,
        num_classes=num_classes)