add_subdirectory( converter )
add_subdirectory( thumbnail )
add_subdirectory( kthesaurus )
add_subdirectory( quickprint )

# strigi ships with the odfsaxanalyzer which does already fetch the meta-data of
# odf files. Also the current implementations of the analyzers are deactivated
# anyway. So, what would be needed is to connect somehow with KoStore or hook
# in the koffice-filters another way somehow.
#add_subdirectory( strigi-analyzer )
