
cd bin

javah -jni org.nanosite.warp.jni.Warp
javah -jni org.nanosite.warp.jni.WarpFunctionBlock
javah -jni org.nanosite.warp.jni.WarpBehavior
javah -jni org.nanosite.warp.jni.WarpStep

mkdir ../../org.nanosite.warp/src-gen
cp *.h ../../org.nanosite.warp/src-gen
