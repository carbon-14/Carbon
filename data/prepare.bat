cd ../bin/win32/Release
TextureCompiler color -cmp -mipmap ../../../cache/carbon_c.btx ../../../data/carbon_c.png
TextureCompiler normal -cmp -mipmap ../../../cache/carbon_n.btx ../../../data/carbon_n.png

TextureCompiler color -cmp -mipmap ../../../cache/crack_c.btx ../../../data/crack_c.png
TextureCompiler normal -cmp -mipmap ../../../cache/crack_n.btx ../../../data/crack_n.png

MeshCompiler -gen_ts -cmp_pos -cmp_vec -cmp_uvs ../../../cache/sphere.bmh ../../../data/sphere.dae

MeshCompiler -cmp_pos -cmp_vec -cmp_uvs ../../../cache/sibenik.bmh ../../../data/sibenik.dae

pause