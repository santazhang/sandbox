# How to train a tree recognizer

1. Properly build the project (build-deps, make)

(Run the following steps in training dir)

2. Copy (or soft link) raw source data (.jp2 and .las files) into 01_source_data/
   (NOTE: currently presumes 0.15 resolution .jp2 images)
3. Run step1_prepare_tree_tagging_data.py
4. Tag tree and not-tree regions with tag-image application
5. Run step2_prepare_training_data.py
6. (Optional) Manually preview images under 04_training_data_preview/, delete the bad ones
7. Run step3_rebuild_training_database.py
8. Run step4_do_training.py
9. results will be in 06_training_output/
