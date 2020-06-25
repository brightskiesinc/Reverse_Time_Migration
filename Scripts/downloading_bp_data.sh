echo "This script will download the BP velocity model, density model and all shot files"
echo "The size of the data to be downloaded is around 8 Gbyte, after extraction it will take 13 GByte"
cd data

wget http://s3.amazonaws.com/open.source.geoscience/open_data/bpvelanal2004/shots0001_0200.segy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bpvelanal2004/shots0201_0400.segy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bpvelanal2004/shots0401_0600.segy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bpvelanal2004/shots0601_0800.segy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bpvelanal2004/shots0801_1000.segy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bpvelanal2004/shots1001_1200.segy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bpvelanal2004/shots1201_1348.segy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bpvelanal2004/density_z6.25m_x12.5m.segy.gz
wget http://s3.amazonaws.com/open.source.geoscience/open_data/bpvelanal2004/vel_z6.25m_x12.5m_exact.segy.gz

gunzip shots0001_0200.segy.gz
gunzip shots0201_0400.segy.gz
gunzip shots0401_0600.segy.gz
gunzip shots0601_0800.segy.gz
gunzip shots0801_1000.segy.gz
gunzip shots1001_1200.segy.gz
gunzip shots1201_1348.segy.gz
gunzip density_z6.25m_x12.5m.segy.gz
gunzip vel_z6.25m_x12.5m_exact.segy.gz


