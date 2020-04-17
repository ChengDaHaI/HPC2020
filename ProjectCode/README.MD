# Introduction
The code collection of HPC2020Spring Project

# Requirements
- Pytorch >= 1.0 
- Python > 3.6 
  
# Run C-SGD
```
python Training_cifar10_resnet18.py  --gpu_devices 0  --batch_size 256 --max_epochs 100
```

# Run D-SGD

```
# node 1
python DistTraining_cifar10_resnet18.py --rank 0 --world_size 4 --gpu_devices 0  --batch_size 256 --max_epochs 100 --init_method file:///scratch/cheng.hai/HPC2020/dist_file
# node 2
python DistTraining_cifar10_resnet18.py --rank 1 --world_size 4 --gpu_devices 0  --batch_size 256 --max_epochs 100 --init_method file:///scratch/cheng.hai/HPC2020/dist_file
# node 3
python DistTraining_cifar10_resnet18.py --rank 2 --world_size 4 --gpu_devices 0  --batch_size 256 --max_epochs 100 --init_method file:///scratch/cheng.hai/HPC2020/dist_file
# node 4
python DistTraining_cifar10_resnet18.py --rank 3 --world_size 4 --gpu_devices 0  --batch_size 256 --max_epochs 100 --init_method file:///scratch/cheng.hai/HPC2020/dist_file
``` 

#Run FL
```
python main_fed.py --num_users 4 --frac 1.0 --momentum 0.9 --local_ep 1 --local_bs 256 --epochs 100
```