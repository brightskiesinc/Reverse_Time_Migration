echo "This script will download the BP velocity model, density model and a single shots file containing shots from 601 to 800"
echo "The size of the data to be downloaded is around 1 Gbyte, after extraction it will be around 2 GByte"
cd data

wget http://s3.amazonaws.com/open.source.geoscience/open_data/bpvelanal2004/shots0601_0800.segy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bpvelanal2004/density_z6.25m_x12.5m.segy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bpvelanal2004/vel_z6.25m_x12.5m_exact.segy.gz

gunzip shots0601_0800.segy.gz
gunzip density_z6.25m_x12.5m.segy.gz
gunzip vel_z6.25m_x12.5m_exact.segy.gz


