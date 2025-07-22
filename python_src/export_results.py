def export_classification_report_csv(
        cr,
        target_directory,
        filename):
    """
    Export classification report into CSV

    Args:
        cr - classification report as str or dict
        target_directory - target directory
        filename - filename of csv file
    """
    # Import necessary libraries
    import csv
    import os

    csv_filepath = os.path.join(
        target_directory,
        "{}.csv".format(filename))
    
    if (type(cr) == str):
        f = open(csv_filepath, 'w')
        f.write(cr)
        f.close()
    elif (type(cr) == dict):
        with open(csv_filepath, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile, delimiter=',')

            for key, value in cr.items():
                writer.writerow([key, value])
    else:
        assert(False)

def export_results(
    y_pred,
    y_test,
    num_classes,
    output_directory,
    filename_prefix=""):
    """ Create confusion matrix figures from lists

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
    import os
    import tensorflow as tf
    import seaborn as sn
    import matplotlib.pyplot as plt
    from sklearn.metrics import classification_report

    # Make necessary directory if necessary
    os.makedirs(output_directory, exist_ok=True)

    # Constants
    CONFUSION_MATRIX_FIGURE_DPI=600
    TITLE="Confusion Matrix"
    FIGURE_FILENAME_WITH_ANNOTATION = "{}_confusion_matrix_annot.png".format(filename_prefix)
    FIGURE_FILENAME_NO_ANNOTATION = "{}_confusion_matrix_no_annot.png".format(filename_prefix)

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

    # Export classification report as a string
    cr_string = classification_report(
        y_true=y_test,
        y_pred=y_pred,
        labels=None,
        digits=4,
        output_dict=False,
    )
    export_classification_report_csv(
        cr_string,
        output_directory,
        filename="{}_str_model_cr".format(filename_prefix)
    )
