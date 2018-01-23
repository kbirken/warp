
cd bin

javah -jni org.nanosite.warp.jni.Warp
javah -jni org.nanosite.warp.jni.WarpPool
javah -jni org.nanosite.warp.jni.WarpFunctionBlock
javah -jni org.nanosite.warp.jni.WarpBehavior
javah -jni org.nanosite.warp.jni.WarpStep

mkdir ../../../cpp/org.nanosite.warp/src-gen
cp *.h ../../../cpp/org.nanosite.warp/src-gen
