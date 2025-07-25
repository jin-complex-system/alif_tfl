def export_graph(
        y_pred,
        y_true,
        num_classes,
        output_directory,
        labels=None
    ):
    # Import libraries
    import numpy as np
    from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score, confusion_matrix
    import matplotlib.pyplot as plt
    import seaborn as sns
    import os

    # Constants
    FIGURE_DPI = 900

    # Convert lists to numpy arrays for metrics calculation
    all_true_labels = np.array(y_true)
    all_pred_labels = np.array(y_pred)


    '''
    MULTI CLASS 
    '''

    # # **Remove elements where true_label >= num_classes**
    # valid_indices = all_true_labels < num_classes  # Boolean mask
    # all_true_labels = all_true_labels[valid_indices]
    # all_pred_labels = all_pred_labels[valid_indices]

    # Calculate classification metrics
    accuracy = accuracy_score(all_true_labels, all_pred_labels)
    precision = precision_score(all_true_labels, all_pred_labels, average='weighted')
    recall = recall_score(all_true_labels, all_pred_labels, average='weighted')
    f1 = f1_score(all_true_labels, all_pred_labels, average='weighted')
    cm = confusion_matrix(all_true_labels, all_pred_labels)

    # # Print metrics
    # print("Evaluation Results:")
    # print(f"Accuracy: {accuracy:.4f}")
    # print(f"Precision (Weighted): {precision:.4f}")
    # print(f"Recall (Weighted): {recall:.4f}")
    # print(f"F1-Score (Weighted): {f1:.4f}")

    class_accuracies = []
    for i in range(0, num_classes):

        if (len(labels) <= i):
            continue
        else:
            acc=np.sum(cm[i, i]) / np.sum(cm[i, :]) if np.sum(cm[i, :]) > 0 else 0
            class_accuracies.append((labels[i], f"{acc:.2%}"))
            print(f"Class {labels[i]}: ACC ({acc:.2%})")

    # Create figure 
    fig, axes = plt.subplots(
        1,
        3,
        figsize=(22, 5),
        dpi=FIGURE_DPI,
        gridspec_kw={'wspace': 0.3})
    
    # 1. Per-Class Accuracy Table
    table_data = [["Class", "Accuracy"]] + class_accuracies
    table = axes[0].table(
        cellText=table_data,
        cellLoc='center',
        loc='center',
        bbox=[0, 0, 1, 1]
    )
    table.auto_set_font_size(False)
    table.set_fontsize(12)
    for key, cell in table.get_celld().items():
        cell.set_height(0.12)
        cell.set_text_props(weight='bold' if key[0] == 0 else 'normal')
    axes[0].axis('off')
    axes[0].set_title("Per-Class Accuracy", fontsize=14, weight='bold')

    # 2. Metrics Table
    metrics_table = [
        ["Accuracy", f"{accuracy:.4f}"],
        ["Precision", f"{precision:.4f}"],
        ["Recall", f"{recall:.4f}"],
        ["F1-Score", f"{f1:.4f}"],
    ]
    table2 = axes[1].table(
        cellText=metrics_table,
        colLabels=["Metric", "Multi-Class"],
        cellLoc='center',
        loc='center',
        bbox=[0, 0, 1, 1]
    )
    table2.auto_set_font_size(False)
    table2.set_fontsize(13)
    for cell in table2.get_celld().values():
        cell.set_height(0.22)
    axes[1].axis('off')
    axes[1].set_title("Overall Metrics", fontsize=14, weight='bold')

    # 3. Multi-Class Confusion Matrix
    sns.heatmap(
        cm,
        annot=True,
        fmt='d',
        cmap='Blues',
        square=True,
        xticklabels=labels,
        yticklabels=labels,
        ax=axes[2],
        cbar=False,
        annot_kws={"size": 9}
    )
    axes[2].set_title("Multi-Class Confusion Matrix", fontsize=14, weight='bold')
    axes[2].set_xlabel("Predicted", fontsize=12)
    axes[2].set_ylabel("True", fontsize=12)
    axes[2].tick_params(axis='x', rotation=90)
    axes[2].tick_params(axis='y', rotation=0)

    # Save the figure in the fig folder
    output_filepath = os.path.join(output_directory, "eval.png")

    fig.tight_layout(pad=2.0)
    fig.savefig(
        output_filepath,
        dpi=FIGURE_DPI,
        bbox_inches='tight')
    plt.close()

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

def _main(
        target_directory,
        output_directory,
        num_classes,
        negative_ids,
        labels):
    # Check parameters
    assert (
        negative_ids is None or 
        len(negative_ids) == 0 or 
        len(negative_ids) < num_classes)

    # Import libraries
    import os
    import pickle

    y_pred_filepath = os.path.join(target_directory, 'y_pred_file')
    y_test_filepath = os.path.join(target_directory, 'y_test_file')

    y_pred = []
    y_test = []

    with open(y_pred_filepath, 'rb') as fp:
        y_pred = pickle.load(fp)
    with open(y_test_filepath, 'rb') as fp:
        y_test = pickle.load(fp)
    assert(len(y_pred) == len(y_test) and len(y_pred) > 0)

    y_pred_filtered = list()
    y_test_filtered = list()

    # Filter out negative ids
    # Note that class_ids saved do not considered negative_ids
    if (negative_ids is not None and 
        len(negative_ids) != 0 and
        len(negative_ids) < num_classes):

        for iterator in range(0, len(y_pred)):
            append_element = True
            subtract_test_id = 0
            for negative_id in negative_ids:
                if y_test[iterator] == negative_id:
                    append_element = False
                elif y_test[iterator] > negative_id:
                    subtract_test_id = len(negative_ids)

            if append_element:
                y_test_filtered.append(y_test[iterator] - subtract_test_id)
                y_pred_filtered.append(y_pred[iterator] - 0)
                
        assert (len(y_test_filtered) == len(y_pred_filtered))
    else:
        y_test_filtered = y_test
        y_pred_filtered = y_pred
        print("No negative ids")

    assert (len(y_test_filtered) == len(y_pred_filtered))

    export_results(
        y_pred=y_pred_filtered,
        y_test=y_test_filtered,
        num_classes=num_classes,
        output_directory=output_directory,
        filename_prefix="edge",
    )
    export_graph(
        y_pred=y_pred_filtered,
        y_true=y_test_filtered,
        num_classes=num_classes,
        output_directory=output_directory,
        labels=labels,
    )

if __name__ == '__main__':
    import os

    urbansound_directory = os.path.join("_my_results", "Urbansound_HE_Pre")
    urbansound_labels = [
        "AIR_CONDITIONER",
        "HORN",
        "CHILDREN",
        "DOG",
        "DRILLING",
        "ENGINE", 
        # "JACKHAMMER",
        "SIREN",
        "MUSIC",
        "FIREARM",
    ]
    urbansound_negative_ids = [6]
    urbansound_num_classes = len(urbansound_labels)

    # No NPU version
    urbansound_no_NPU_directory = "{}_no_NPU".format(urbansound_directory)
    _main(
        target_directory=urbansound_no_NPU_directory,
        output_directory=os.path.join(urbansound_no_NPU_directory, "fig"),
        num_classes=urbansound_num_classes,
        negative_ids=urbansound_negative_ids,
        labels=urbansound_labels,
    )
    # NPU version
    urbansound_NPU_directory = "{}_NPU".format(urbansound_directory)
    _main(
        target_directory=urbansound_NPU_directory,
        output_directory=os.path.join(urbansound_NPU_directory, "fig"),
        num_classes=urbansound_num_classes,
        negative_ids=urbansound_negative_ids,
        labels=urbansound_labels,
    )

    orbiwise_directory = os.path.join("_my_results", "Orbiwise_HE_Pre")
    orbiwise_labels = [
        "Car", # c
        "motorcycle",
        "truck",
        "bus", # c
        "train", # c
        "Vehicle_horn", # c
        "siren",  # c
        "airplane", # c
        "helicopter",
        "walk_footsteps", # c
        "human_voice", # c
        "crowd",   # c
        "people_screaming",
        "women_screaming",
        "music",   # c
        "singing", # c
        "hammer",  # c
        "jackhammer",
        "chainsaw",
    ]
    orbiwise_negative_ids = [19]
    orbiwise_num_classes = 20

    # No NPU version
    orbiwise_no_NPU_directory = "{}_no_NPU".format(orbiwise_directory)
    _main(
        target_directory=orbiwise_no_NPU_directory,
        output_directory=os.path.join(orbiwise_no_NPU_directory, "fig"),
        num_classes=orbiwise_num_classes,
        negative_ids=orbiwise_negative_ids,
        labels=orbiwise_labels,
    )

    # NPU version
    orbiwise_NPU_directory = "{}_NPU".format(orbiwise_directory)
    _main(
        target_directory=orbiwise_NPU_directory,
        output_directory=os.path.join(orbiwise_NPU_directory, "fig"),
        num_classes=orbiwise_num_classes,
        negative_ids=orbiwise_negative_ids,
        labels=orbiwise_labels,
    )
    print("Done")
