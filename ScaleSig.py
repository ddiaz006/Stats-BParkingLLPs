import os
import argparse

def process_card_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
    
    if lines[0].startswith('#SF:'):
        print(f'Scale Factor already applied in: {file_path}')
        return
    
    # Identify the rate line and calculate the scale factor (SF)
    rate_line_index = next(i for i, line in enumerate(lines) if line.strip().startswith('rate'))
    rate_values = lines[rate_line_index].split()[1:]  # skip the 'rate' keyword
    indices_to_use = [0, 2, 4, 6]

    values_to_use = [float(rate_values[i]) for i in indices_to_use]
    #sf = 1.0/sum(values_to_use) / len(values_to_use)
    sf = 1.
    if values_to_use[0] > 0:
      sf = 2.0/values_to_use[0]

    new_rate_values = ['rate']
    new_scaled_rate_values = []
    for i, value in enumerate(rate_values):
        float_value = float(value)  # Convert every value to float first
        if i%2 == 0:
            new_value = float_value * sf
            new_rate_values.append(f'{new_value:.8f}')
            new_scaled_rate_values.append(new_value)  # Append scaled value
        else:
            new_rate_values.append(value)
            new_scaled_rate_values.append(float_value)  # Append unscaled value

    lines[rate_line_index] = ' '.join(new_rate_values) + '\n'

    # Update mc_stats lines based on new scaled rate values
    for i, line in enumerate(lines):
        if 'mc_stats' in line:
            parts = line.split()
            if 'gmN' in parts:
                gmN_index = parts.index('gmN') 
                gmN_value = int(parts[gmN_index+1])  # the gmN value
                new_mc_stats_values = parts[:gmN_index+2]  # keeps initial parts unchanged including gmN and its value
                # Start processing data just after gmN value, correct index is gmN_index + 2 because we skip gmN and its integer value
                data_start_index = gmN_index + 2
                # Iterate through remaining parts aligned with rate values
                for j in range(data_start_index, len(parts)):
                    rate_index = j - data_start_index  # corresponding index in new_scaled_rate_values
                    if rate_index < len(new_scaled_rate_values):
                        rate_val = new_scaled_rate_values[rate_index]
                        if rate_val != 1 and parts[j] != '-':
                            adjusted_val = float(rate_val) / gmN_value
                            new_mc_stats_values.append(f'{adjusted_val:.9f}')
                        else:
                            new_mc_stats_values.append(parts[j])
                    else:
                        # Handling the case where mc_stats line has more entries than new_scaled_rate_values
                        new_mc_stats_values.append(parts[j])
                        
                lines[i] = ' '.join(new_mc_stats_values) + '\n'
        
    lines.insert(0, f'#SF: {sf:.5E}\n')

    with open(file_path, 'w') as file:
        file.writelines(lines)

def update_directory(parent_directory):
    # Traverse through the directory
    for root, dirs, files in os.walk(parent_directory):
        for file_name in files:
            if file_name == 'card.txt':
                file_path = os.path.join(root, file_name)
                print(f'Processing file: {file_path}')
                process_card_file(file_path)
#def update_directory(parent_directory):
#    # Specific file path to process
#    specific_file_path = os.path.join(parent_directory, 'Lim_WP19_IT/SigPiPlusPiMinus_M0p3_low_ctau300/card.txt')
#
#    # Check if the specific file exists and process only this file
#    if os.path.exists(specific_file_path):
#        print(f'Processing file: {specific_file_path}')
#        process_card_file(specific_file_path)
#    else:
#        print(f'Specified file does not exist: {specific_file_path}')



def main():
    parser = argparse.ArgumentParser(description='Scale values in card.txt files based on specified indices and adjust mc_stats lines.')
    parser.add_argument('--dir', type=str, required=True, help='Path to the parent directory containing card.txt files.')
    args = parser.parse_args()
    
    update_directory(args.dir)

if __name__ == '__main__':
    main()

