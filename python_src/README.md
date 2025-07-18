# Instructions

1. Clone the git repository inside your WSL and navigate into the Python script directory
```bash
git clone https://github.com/jin-complex-system/alif_tfl
cd alif_tfl/python_src
```
2. Active Python virtual environment
```bash
mkdir venv
python -m venv venv/.
chmod +777 venv/bin/activate
source venv/bin/activate
```
3. Install necessary requirements if needed
```bash
pip install -r requirements.txt
```
3. To parse output from the SD card, copy the output directory to `python_src`:
```bash
cp -r <sd_card_location>/out_A <alif_tfl_repo_path>/python_src
```
- Example is using `out_A`. If needed, change the directory inside `parse_output.py` before running

4. Run [`parse_output.py`](parse_output.py) and read the output directory
```bash
python3 parse_output.py
cd _my_results
```