import os
import librosa
import numpy as np
import glob

def convert_audio_file_to_binary(
    input_filepath,
    output_directory,
    target_endian,
    target_dtype):
    """ Convert audio file to binary file

    Args:
        input_filepath - 
        output_directory - 
        target_endian - Endianness of target
        target_dtype - targetted type

    """
    # Check parameters
    assert (len(input_filepath) > 0)
    assert (len(output_directory) > 0)
    assert (
        target_dtype is np.int16 or 
        target_dtype is np.int32 or
        target_dtype is np.float32)

    # Get output filename
    os.makedirs(output_directory, exist_ok=True)
    filename = os.path.basename(input_filepath)
    output_filepath = os.path.join(output_directory, filename)

    # Load the audio file using librosa
    # Resamples to 44.1 kHz
    audio_samples, _ = librosa.load(filename, sr=44100, mono=True)

    # Convert to desired type and endianness
    if target_dtype is np.float32:
        audio_converted = audio_samples
    elif (target_dtype is np.int16 or target_dtype is np.int32):
        audio_converted = (audio_samples * np.iinfo(target_dtype).max).astype(target_dtype)
    else:
        # Unsupported
        assert (False)
    assert (audio_converted is not None)

    audio_converted.tofile(output_filepath)

def convert_audio_files_to_binary(
    input_directory,
    output_directory,
    target_endian,
    target_dtype,
    file_extension="wav"):
    """ Convert audio files to binary files
    
    Args:
        input_directory - 
        output_directory - 
        target_endian - Endianness of target
        target_dtype - targetted type
        file_extension - file extension to be converted
    """

    # Check parameters
    assert (len(input_directory) > 0)
    assert (len(output_directory) > 0)
    assert (len(file_extension) > 0)

    # Iterate through the directory
    for input_filepath in glob.glob(os.path.join(input_directory, f'*.{file_extension}')):
        convert_audio_file_to_binary(
            input_filepath=input_filepath,
            output_directory=output_directory,
            target_endian=target_endian,
            target_dtype=target_dtype)
        
def _main():
    INPUT_DIRECTORY = "_target_audio_files"
    OUTPUT_DIRECTORY = "ow_audio"
    ALIF_ENDIAN = "big"
    target_dtype = np.int16

    convert_audio_files_to_binary(
        input_directory=INPUT_DIRECTORY,
        output_directory=OUTPUT_DIRECTORY,
        target_endian=ALIF_ENDIAN,
        target_dtype=target_dtype,
        file_extension=".wav"
    )

if __name__ == '__main__':
    _main()

