# 사용법

작업 디렉토리는 `8x8_Data`의 부모 디렉토리 기준입니다.

(이 경우 `Image_generation`) 

작업 디렉토리의 하위 디렉토리로 `8x8_Data`가 있고, 이름별로 데이터가 저장되어 있습니다.

1. `split_csv_to_img.py` 
    
    여러 data의 모임인 csv파일을 더 작은 2차원 csv파일로 분해합니다. 모듈을 실행시키면 현재 작업 디렉토리 아래의 모든 csv파일을 분해해 `some_name/images`와 `some_name/test_images`에 저장합니다.

    반복하여 실행되었을 때 이미 분해된 csv 파일을 분해하지 않도록 `images`, `samples`, `results`가 포함된 디렉토리에는 접근하지 않습니다.


2. `train_model.py`

    Diffusion 모델을 만들어 30000 step 동안 학습합니다. `run_exp`의 인자로 `8x8_Data` 아래의 `이름 폴더명`을 입력합니다.
    
    ```python
    if __name__ == "__main__":
        run_exp("Lee1")
    ```

    훈련 결과는 1000 step마다 `이름 폴더명/results`에 pt 파일로 저장됩니다.


3. `generate_samples.py`
   
    모델을 불러오고, 데이터를 생성합니다. `이름 폴더명`, `샘플 크기`, `csv로 저장 여부`, `normalize 여부`를 지정합니다.

    `csv로 저장 == True` 이면 생성된 데이터를 (`샘플 크기`, 64)의 형태로  

     `작업 디렉토리/8x8_Data/gen_samples/이름_gen_samples_raw.csv`
     
     or

      `작업 디렉토리/8x8_Data/gen_samples/이름_gen_samples_normalized.csv`
     
     에 저장합니다.

