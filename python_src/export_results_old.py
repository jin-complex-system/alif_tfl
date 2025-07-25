import os
from export_results import export_results, export_graph

def parse_file_for_predictions(
    target_filepath):
    """
    """

    results = list()

    # Open the file itself to get the predicted label
    with open(
        target_filepath,
        mode='rb',
        encoding=None) as fh:
        for line in fh:
            for byte_element in line:
                results.append(int(byte_element))
    return results

def process_results(
    input_directory,
    model_directories,
    output_directory,
    core_types,
    labels,
    num_classes,
    # negative_ids,
):
    """ Parse through the file
    """

    for core_type in core_types:
        for model_directory in model_directories:
            model_directory = "{}_{}_vela".format(
                model_directory, core_type)
            
            target_directory = os.path.join(
                input_directory,
                model_directory)
            
            y_pred_filepath = os.path.join(target_directory, '{}_y_pred.P'.format(model_directory))
            y_test_filepath = os.path.join(target_directory, '{}_y_test.P'.format(model_directory))

            y_pred = parse_file_for_predictions(target_filepath=y_pred_filepath)
            y_test = parse_file_for_predictions(target_filepath=y_test_filepath)

            result_directory = os.path.join(output_directory, model_directory)

            # Only grab results according to y_pred
            y_test = y_test[0:len(y_pred)]
            os.makedirs(result_directory, exist_ok=True)

            export_results(
                y_pred=y_pred,
                y_test=y_test,
                num_classes=num_classes,
                output_directory=result_directory,
                filename_prefix="edge_old",
            )
            export_graph(
                y_pred=y_pred,
                y_true=y_test,
                num_classes=num_classes,
                output_directory=result_directory,
                labels=labels,
            )


def _main():
    input_directory = "_output_old"
    output_directory = "_my_results"
    core_types = [
        "HE", "HP"
    ]

    # Urbansound
    urbansound_model_results = [
        "CNN_litert",
        "DTFT_SAC_Q",
        "DTFT_Q",
        "DTFT_SAC_Q",
    ]
    urbansound_num_classes = 10
    urbansound_labels = [
        "AIR_CONDITIONER",
        "HORN",
        "CHILDREN",
        "DOG",
        "DRILLING",
        "ENGINE", 
        "JACKHAMMER",
        "SIREN",
        "MUSIC",
        "FIREARM",
    ]
    process_results(
        input_directory=input_directory,
        model_directories=urbansound_model_results,
        output_directory=output_directory,
        core_types=core_types,
        labels=urbansound_labels,
        num_classes=urbansound_num_classes)

    # Orbiwse
    orbiwise_model_results = [
        "model_orbw_19_Q",
    ]
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
    # orbiwise_negative_ids = [19]
    orbiwise_num_classes = 20
    process_results(
        input_directory=input_directory,
        model_directories=orbiwise_model_results,
        output_directory=output_directory,
        core_types=core_types,
        labels=orbiwise_labels,
        num_classes=orbiwise_num_classes)

if __name__ == "__main__":
    _main()
