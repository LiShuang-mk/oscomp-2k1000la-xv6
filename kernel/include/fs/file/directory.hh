// #include "fs/file/normal.hh"
// #include "klib/common.hh"

// namespace fs
// {
//     class directory : public normal_file
//     {

//         public:
//             directory( FileAttrs attrs, dentry *den ) : normal_file( attrs, den ), _den( den ) { dup(); new ( &_stat ) Kstat( den ); }
//             directory( dentry *den ) : normal_file(FileAttrs( FileTypes::FT_DIRECT, 0777 ), den), _den( den ) { dup(); new ( &_stat ) Kstat( den ); }
//             int read( uint64 buf, size_t len, int off = -1, bool upgrade = false ) override 
//                                             { log_error(" Directory is not allowed to write"); return -1;};
//             int write( uint64 buf, size_t len ) override 
//                                             { log_error("Directory is not allowed to read"); return -1; };
//     };
// }